#include "MainGrid.h"
#include "Bucket.h"
#include "Game.h"
#include "math/MathUtils.h"
#include "Urho3D/Resource/Image.h"
#include "objects/static/Static.h"
#include "objects/unit/Unit.h"
#include <array>
#include <unordered_set>
#include "objects/building/Building.h"
#include "debug/DebugLineRepo.h"
#include "simulation/env/Environment.h"
#include "database/DatabaseCache.h"


MainGrid::MainGrid(const short resolution, const float size): Grid(resolution, size) {
	complexData = new ComplexBucketData[sqResolution];
	auto quater = fieldSize / 4;
	posInBucket2 = {Urho3D::Vector2(quater, quater), Urho3D::Vector2(-quater, -quater)};
	posInBucket3 = {
		Urho3D::Vector2(quater, quater), Urho3D::Vector2(-quater, quater),
		Urho3D::Vector2(-quater, 0)
	};
	posInBucket4 = {
		Urho3D::Vector2(quater, quater), Urho3D::Vector2(-quater, quater),
		Urho3D::Vector2(quater, -quater), Urho3D::Vector2(-quater, -quater)
	};
	pathConstructor = new PathFinder(resolution, size, complexData);

	DebugLineRepo::init(DebugLineType::MAIN_GRID);
}

MainGrid::~MainGrid() {
	delete[] complexData;
	delete pathConstructor;
}

void MainGrid::prepareGridToFind() const {
	for (int i = 0; i < sqResolution; ++i) {
		updateNeighbors(i);
	}
	pathConstructor->prepareGridToFind();
}

bool MainGrid::validateAdd(Static* object) const {
	const auto pos = object->getPosition();
	return validateAdd(object->getGridSize(), Urho3D::Vector2(pos.x_, pos.z_));
}

bool MainGrid::validateAdd(const Urho3D::IntVector2& size, Urho3D::Vector2& pos) const {
	const auto sizeX = calculateSize(size.x_, calculator.getIndex(pos.x_));
	const auto sizeZ = calculateSize(size.y_, calculator.getIndex(pos.y_));

	for (int i = sizeX.x_; i < sizeX.y_; ++i) {
		for (int j = sizeZ.x_; j < sizeZ.y_; ++j) {
			if (!calculator.isValidIndex(i, j)) {
				return false;
			}
			const int index = calculator.getIndex(i, j);
			if (!(calculator.isValidIndex(index) && complexData[index].isUnit())) {
				return false;
			}
		}
	}

	return true;
}

Urho3D::Vector2 MainGrid::repulseObstacle(Unit* unit) const {
	auto index = calculator.indexFromPosition(unit->getPosition());
	auto &data = complexData[index];
	
	Urho3D::Vector2 sum;
	if (index != unit->getIndexToInteract()
		&& data.isUnit()
		&& data.allNeightOccupied()) {
		int counter = 0;

		for (auto i : closeIndexProvider.getTabIndexes(index)) {
			if (!data.ifNeightIsFree(i)) {
				sum += calculator.getCenter(index + closeIndexProvider.getIndexAt(i));
				++counter;
			}
		}
		sum /= counter;
		sum -= Urho3D::Vector2(unit->getPosition().x_, unit->getPosition().z_);
	}
	return -sum;
}

void MainGrid::invalidateCache() const {
	pathConstructor->invalidateCache();
}

void MainGrid::updateSurround(Static* object) const {
	if (object->isAlive()) {
		std::unordered_set<int> indexes;
		for (auto index : object->getOccupiedCells()) {
			for (auto inIndex : closeIndexProvider.get(index)) {
				auto newIndex = index + inIndex;
				indexes.emplace(newIndex);
			}
		}
		for (auto index : object->getOccupiedCells()) {
			indexes.erase(index);
		}
		auto& surroundCells = object->getSurroundCells();
		surroundCells.insert(surroundCells.begin(), indexes.begin(), indexes.end());
	} //TODO else czy cos trzeba usunac?
}

Urho3D::Vector2 MainGrid::getPositionInBucket(int index, char max, char i) {
	auto center = calculator.getCenter(index);
	switch (max) {
	case 1:
		return center;
	case 2:
		return posInBucket2[i] + center;
	case 3:
		return posInBucket3[i] + center;
	case 4:
		return posInBucket4[i] + center;
	}
}

bool MainGrid::cellInStates(int index, std::vector<CellState>& cellStates) const {
	auto type = complexData[index].getType();
	for (auto cellState : cellStates) {
		if (type == cellState) {
			return true;
		}
	}
	return false;
}

void MainGrid::updateCell(int index, char val, CellState cellState) const {
	complexData[index].updateSize(val, cellState);
}

bool MainGrid::belowCellLimit(int index) const {
	return complexData[index].belowCellLimit();
}

char MainGrid::getNumberInState(int index, UnitState state) const {
	auto pred = [state](Physical* p) { return dynamic_cast<Unit*>(p)->getState() == state; };

	auto& content = getContentAt(index);
	return std::count_if(content.begin(), content.end(), pred);
}

char MainGrid::getOrdinalInState(Unit* unit, UnitState state) const {
	const auto index = unit->getMainCell();
	char ordinal = 0;
	for (auto physical : buckets[index].getContent()) {
		if (physical == unit) {
			return ordinal;
		}
		if (dynamic_cast<Unit*>(physical)->getState() == state) {
			++ordinal;
		}
	}
	return -1;
}

int MainGrid::getRevertCloseIndex(int center, int gridIndex) const {
	int index = gridIndex - center;
	for (auto i : closeIndexProvider.getTabIndexes(center)) {
		if (closeIndexProvider.getIndexAt(i) == index) {
			//TODO performance
			return i;
		}
	}
	assert(false);
	Game::getLog()->Write(0, "closeIndex miscalculate");
	return 0;
}

void MainGrid::addDeploy(Building* building) const {
	auto optDeployIndex = building->getDeploy();

	if (optDeployIndex.has_value()) {
		complexData[optDeployIndex.value()].setDeploy(building);
	}
}

void MainGrid::removeDeploy(Building* building) const {
	complexData[building->getDeploy().value()].removeDeploy();
}

CellState MainGrid::getCellAt(float x, float z) const {
	return complexData[indexFromPosition({x, z})].getType();
}

int MainGrid::getAdditionalInfoAt(float x, float z) const {
	auto index = indexFromPosition({x, z});

	return complexData[index].getAdditionalInfo();
}

void MainGrid::drawDebug(GridDebugType type) const {
	DebugLineRepo::clear(DebugLineType::MAIN_GRID);
	DebugLineRepo::beginGeometry(DebugLineType::MAIN_GRID);

	switch (type) {
	case GridDebugType::NONE:
		break;
	case GridDebugType::GRID:
	{
		float value = -size / 2;
		for (int i = 0; i < resolution; ++i) {
			DebugLineRepo::drawLine(DebugLineType::MAIN_GRID, {-size / 2, 10, value},
			                        {size / 2, 10, value}, Urho3D::Color::CYAN);
			DebugLineRepo::drawLine(DebugLineType::MAIN_GRID, {value, 10, -size / 2},
			                        {value, 10, size / 2}, Urho3D::Color::CYAN);
			value += fieldSize;
		}
	}
	break;
	case GridDebugType::CELLS_TYPE:
	{
		for (int i = 0; i < sqResolution; ++i) {
			auto center = calculator.getCenter(i);
			std::tuple<bool, Urho3D::Color> info = DebugLineRepo::getInfoForGrid(complexData[i].getType());

			if (std::get<0>(info)) {
				DebugLineRepo::drawLine(DebugLineType::MAIN_GRID, {center.x_, 10, center.y_},
				                        {center.x_, 20, center.y_}, std::get<1>(info));
			}
		}
	}
	break;
	default: ;
	}

	DebugLineRepo::commit(DebugLineType::MAIN_GRID);
}

float MainGrid::getFieldSize() const {
	return calculator.getFieldSize();
}

bool MainGrid::isInLocalArea(const int cell, Urho3D::Vector2& pos) const {
	const auto index = calculator.indexFromPosition(pos);
	if (cell == index) { return true; }
	for (auto value : closeIndexProvider.get(index)) {
		if (cell == index + value) {
			return true;
		}
	}
	return false;
}

bool MainGrid::isEmpty(int inx) const {
	return calculator.isValidIndex(inx) && complexData[inx].getType() == CellState::EMPTY || complexData[inx].getType()
		==
		CellState::DEPLOY;
}

int MainGrid::closestEmpty(int posIndex) const {
	for (auto i : closeIndexProvider.get(posIndex)) {
		if (isEmpty(i + posIndex)) {
			return i + posIndex;
		}
	}
	for (auto i : closeIndexProvider.getSecond(posIndex)) {
		if (isEmpty(i + posIndex)) {
			return i + posIndex;
		}
	}
	return posIndex;
}

void MainGrid::addStatic(Static* object) const {
	if (validateAdd(object)) {
		const auto bucketPos = getCords(object->getMainCell());

		object->setMainCell(calculator.getIndex(bucketPos.x_, bucketPos.y_));

		for (auto index : object->getOccupiedCells()) {
			complexData[index].setStatic(object);
		}

		std::vector<int> toRefresh;

		const auto size = object->getGridSize();
		const auto sizeX = calculateSize(size.x_, bucketPos.x_);
		const auto sizeZ = calculateSize(size.y_, bucketPos.y_);

		for (int i = sizeX.x_ - 1; i < sizeX.y_ + 1; ++i) {
			for (int j = sizeZ.x_ - 1; j < sizeZ.y_ + 1; ++j) {
				if (calculator.isValidIndex(i, j)) {
					const int index = calculator.getIndex(i, j);
					updateNeighbors(index);
					if (!complexData[index].isUnit()) {
						toRefresh.push_back(index);
					} else {
						complexData[index].setEscapeThrought(-1);
					}
				}
			}
		}
		pathConstructor->refreshWayOut(toRefresh);
	}
	updateSurround(object);
}

void MainGrid::removeStatic(Static* object) const {
	//TODO bug poprawic dziwne zygzagki sie robia gdy przechodzi przez// moze invalidate cache?
	object->setMainCell(-1);
	for (auto index : object->getOccupiedCells()) {
		complexData[index].removeStatic();
		updateNeighbors(index);
	}
	for (auto index : object->getSurroundCells()) {
		updateNeighbors(index);
	}
}

std::optional<Urho3D::Vector2> MainGrid::getDirectionFrom(Urho3D::Vector3& position) const {
	int index = calculator.indexFromPosition(position);
	auto &data = complexData[index];
	
	if (!data.isUnit()) {
		int escapeBucket; //=-1
		//auto& neights = complexData[index].getNeightbours();
		if (!data.allNeightOccupied()) {
			float dist = 999999;
			auto center = calculator.getCenter(index);
			for (auto i : closeIndexProvider.getTabIndexes(index)) {
				if (data.ifNeightIsFree(i)) {
					int ni = index + closeIndexProvider.getIndexAt(i);
					float newDist = sqDist(calculator.getCenter(ni), center);
					if (newDist < dist) {
						dist = newDist;
						escapeBucket = ni;
					}
				}
			}
		} else {
			escapeBucket = data.getEscapeBucket();
		}
		if (escapeBucket == -1) {
			return {};
		}
		auto direction = data //TODO Error'this' nie uzywany 
			.getDirectionFrom(position, calculator.getCenter(escapeBucket));

		direction.Normalize();
		return direction;
	}
	return {};
}

std::pair<Urho3D::IntVector2, Urho3D::Vector2> MainGrid::getValidPosition(
	const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const {
	//TODO tu mozna to sporo zoptymalizowac ale pewnie nie ma potrzeby
	const short posX = calculator.getIndex(pos.x_);
	const short posZ = calculator.getIndex(pos.y_);

	const auto sizeX = calculateSize(size.x_, posX);
	const auto sizeZ = calculateSize(size.y_, posZ);

	const int index1 = calculator.getIndex(sizeX.x_, sizeZ.x_);
	const int index2 = calculator.getIndex(sizeX.y_ - 1, sizeZ.y_ - 1);
	const auto center1 = calculator.getCenter(index1);
	const auto center2 = calculator.getCenter(index2);
	const auto newCenter = (center1 + center2) / 2;

	return {{posX, posZ}, newCenter};
}

void MainGrid::updateNeighbors(const int current) const {
	for (auto i : closeIndexProvider.getTabIndexes(current)) {
		const int nI = current + closeIndexProvider.getIndexAt(i);
		if (calculator.isValidIndex(nI)) {
			if (complexData[nI].isUnit()) {
				complexData[current].setNeightFree(i);
			} else if (!complexData[nI].isUnit()) {
				complexData[current].setNeightOccupied(i);
			}
			complexData[current].setCost(i, cost(current, nI));
		}
	}
}

float MainGrid::cost(const int current, const int next) const {
	return (calculator.getCenter(current) - calculator.getCenter(next)).Length();
}

// std::vector<int>* MainGrid::findPath(Urho3D::IntVector2& startV, Urho3D::IntVector2& goalV) {
// 	int start = getIndex(startV.x_, startV.y_);
// 	int goal = getIndex(goalV.x_, goalV.y_);
// 	float min = cost(start, goal);
// 	return pathConstructor->findPath(start, goal, min, min * 2);
// }

std::vector<int>* MainGrid::findPath(int startIdx, const Urho3D::Vector2& aim) const {
	return pathConstructor->findPath(startIdx, aim);
}

std::vector<int>* MainGrid::findPath(const Urho3D::Vector3& from, const Urho3D::Vector2& aim) const {
	return pathConstructor->findPath(from, aim);
}

void MainGrid::drawMap(Urho3D::Image* image) const {
	const auto data = (uint32_t*)image->GetData();
	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			const int index = calculator.getIndex(x, y);
			const int idR = calculator.getIndex(resolution - y - 1, x);
			if (complexData[index].isUnit()) {
				*(data + idR) = 0xFFFFFFFF;
			} else {
				*(data + idR) = 0xFF000000;
			}
		}
	}
}
