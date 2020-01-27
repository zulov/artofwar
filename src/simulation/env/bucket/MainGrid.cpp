#include "MainGrid.h"
#include "Bucket.h"
#include "Game.h"
#include "MathUtils.h"
#include "Urho3D/Resource/Image.h"
#include "objects/static/Static.h"
#include "objects/unit/Unit.h"
#include <array>
#include <unordered_set>
#include "objects/building/Building.h"
#include "debug/DebugLineRepo.h"
#include "simulation/env/Environment.h"
#include "database/DatabaseCache.h"

#define CLOSE_INDEX_LEVEL 3


void MainGrid::initCloseIndexs(char a, std::vector<short>& vector) const {
	for (char i = -a; i <= a; ++i) {
		for (char j = -a; j <= a; ++j) {
			vector.push_back(i * resolution + j);
		}
	}
	std::sort(vector.begin(), vector.end());
	vector.shrink_to_fit();
}

MainGrid::MainGrid(const short _resolution, const float _size): Grid(_resolution, _size) {
	short posX = 0;
	short posZ = 0;

	int i = 0;
	for (auto& closeIndex : closeIndexes) {
		initCloseIndexs(i++, closeIndex);
	}
	for (int i = CLOSE_INDEX_LEVEL - 1; i > 0; --i) {
		closeIndexes[i].erase(std::remove_if(closeIndexes[i].begin(), closeIndexes[i].end(),
		                                     [this,i](short val) {
			                                     auto next = closeIndexes[i - 1];
			                                     return std::find(next.begin(), next.end(), val) != next.end();
		                                     }),
		                      closeIndexes[i].end());
	}

	complexData = new ComplexBucketData[sqResolution];
	for (int i = 0; i < sqResolution; ++i) {
		const float cX = (posX + 0.5) * fieldSize - size / 2;
		const float cZ = (posZ + 0.5) * fieldSize - size / 2;
		complexData[i].setCenter(cX, cZ);

		++posZ;
		if (posZ >= resolution) {
			++posX;
			posZ = 0;
		}
	}
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
	for (int i = 0; i < resolution * resolution; ++i) {
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
			if (!calculator.validIndex(i, j)) {
				return false;
			}
			const int index = calculator.getIndex(i, j);
			if (!(inRange(index) && complexData[index].isUnit())) {
				return false;
			}
		}
	}

	return true;
}

Urho3D::Vector2 MainGrid::repulseObstacle(Unit* unit) {
	auto index = calculator.indexFromPosition(unit->getPosition());

	Urho3D::Vector2 sum;
	if (index != unit->getIndexToInteract()
		&& complexData[index].isUnit()
		&& complexData[index].allNeightOccupied()) {
		int counter = 0;
		for (int i = 0; i < 8; ++i) {
			if (!complexData[index].ifNeightIsFree(i)) {
				sum += complexData[index + closeIndexes[1][i]].getCenter();
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

void MainGrid::updateSurround(Static* object) {
	if (object->isAlive()) {
		std::unordered_set<int> indexes;
		for (auto index : object->getOccupiedCells()) {
			for (auto inIndex : closeIndexes[1]) {
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
	auto center = complexData[index].getCenter();
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
	for (auto cellState : cellStates) {
		if (complexData[index].getType() == cellState) {
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
	char num = 0;
	for (auto&& physical : buckets[index].getContent()) {
		if (static_cast<Unit*>(physical)->getState() == state) {
			++num;
		}
	}
	return num;
}

char MainGrid::getOrdinalInState(Unit* unit, UnitState state) const {
	const auto index = unit->getMainCell();
	char ordinal = 0;
	for (auto&& physical : buckets[index].getContent()) {
		if (physical == unit) {
			return ordinal;
		}
		if (static_cast<Unit*>(physical)->getState() == state) {
			++ordinal;
		}
	}
	return -1;
}

int MainGrid::getRevertCloseIndex(int center, int gridIndex) {
	int index = gridIndex - center;
	for (int i = 0; i < 8; ++i) {
		//TODO performance
		if (closeIndexes[1][i] == index) {
			return i;
		}
	}
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
	auto index = indexFromPosition({x, z});

	return complexData[index].getType();
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
			DebugLineRepo::drawLine(DebugLineType::MAIN_GRID, Urho3D::Vector3(-size / 2, 10, value),
			                        Urho3D::Vector3(size / 2, 10, value), Urho3D::Color::CYAN);
			DebugLineRepo::drawLine(DebugLineType::MAIN_GRID, Urho3D::Vector3(value, 10, -size / 2),
			                        Urho3D::Vector3(value, 10, size / 2), Urho3D::Color::CYAN);
			value += fieldSize;
		}
	}
	break;
	case GridDebugType::CELLS_TYPE:
	{
		for (int i = 0; i < sqResolution; ++i) {
			auto center = complexData[i].getCenter();
			std::tuple<bool, Urho3D::Color> info = DebugLineRepo::getInfoForGrid(complexData[i].getType());

			if (std::get<0>(info)) {
				DebugLineRepo::drawLine(DebugLineType::MAIN_GRID, Urho3D::Vector3(center.x_, 10, center.y_),
				                        Urho3D::Vector3(center.x_, 20, center.y_), std::get<1>(info));
			}
		}
	}
	break;
	default: ;
	}

	DebugLineRepo::commit(DebugLineType::MAIN_GRID);
}

bool MainGrid::validAndFree(short id, int index, std::vector<short>::value_type close) const {
	auto building = Game::getDatabaseCache()->getBuilding(id);

	return calculator.validIndex(index + close)
		&& Game::getEnvironment()->validateStatic(building->size, calculator.getCenter(index + close))
		&& complexData[index + close].isFreeToBuild(id);
}

Urho3D::Vector2 MainGrid::getNewBuildingPos(const Urho3D::Vector2& center, char player, const short id) {
	auto index = calculator.indexFromPosition(center);
	if (complexData[index].isFreeToBuild(id)) {
		return center;
	}
	for (auto close : closeIndexes[1]) {
		if (validAndFree(id, index, close)) {
			return calculator.getCenter(index + close);
		}
	}

	for (auto close : closeIndexes[2]) {
		if (validAndFree(id, index, close)) {
			return calculator.getCenter(index + close);
		}
	}
	Game::getLog()->Write(0, "AI wrong POS");
	return center; //TODO bug optional
}

bool MainGrid::isInLocalArea(int cell, Urho3D::Vector2& pos) {
	const auto index = calculator.indexFromPosition(pos);
	if (cell == index) { return true; }
	for (auto value : closeIndexes[1]) {
		if (cell == index + value) {
			return true;
		}
	}
	return false;
}

bool MainGrid::isEmpty(int inx) const {
	return calculator.validIndex(inx) && complexData[inx].getType() == CellState::EMPTY || complexData[inx].getType() ==
		CellState::DEPLOY;
}

int MainGrid::closestEmpty(int posIndex) {
	//TODO improve closest? skorzystac z escape?
	int bestIndex = posIndex;
	double closest = 99999;
	auto vec = closeIndexes[1];
	for (auto index : vec) {
		if (isEmpty(index + posIndex)) {
			return index + posIndex;
		}
	}
	for (auto index : closeIndexes[2]) {
		if (isEmpty(index + posIndex)) {
			return index + posIndex;
		}
	}
	return posIndex;
}

void MainGrid::addStatic(Static* object) {
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
				if (calculator.validIndex(i, j)) {
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

Urho3D::Vector2* MainGrid::getDirectionFrom(Urho3D::Vector3& position) {
	int index = calculator.indexFromPosition(position);
	if (!complexData[index].isUnit()) {
		int escapeBucket; //=-1
		//auto& neights = complexData[index].getNeightbours();
		if (!complexData[index].allNeightOccupied()) {
			float dist = 999999;
			for (int i = 0; i < 8; ++i) {
				if (complexData[index].ifNeightIsFree(i)) {
					int ni = index + closeIndexes[1][i];
					float newDist = sqDist(complexData[ni].getCenter(),
					                       complexData[index].getCenter());
					if (newDist < dist) {
						dist = newDist;
						escapeBucket = ni;
					}
				}
			}
		} else {
			escapeBucket = complexData[index].getEscapeBucket();
		}
		if (escapeBucket == -1) {
			return new Urho3D::Vector2;
		}
		Urho3D::Vector2* direction = complexData[index] //TODO Error'this' nie uzywany 
			.getDirectrionFrom(position, complexData[escapeBucket]);

		direction->Normalize();
		return direction;
	}
	return nullptr;
}

Urho3D::Vector2 MainGrid::getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const {
	//TODO tu mozna to sporo zoptymalizowac ale pewnie nie ma potrzeby
	const short posX = calculator.getIndex(pos.x_);
	const short posZ = calculator.getIndex(pos.y_);

	const auto sizeX = calculateSize(size.x_, posX);
	const auto sizeZ = calculateSize(size.y_, posZ);

	const int index1 = calculator.getIndex(sizeX.x_, sizeZ.x_);
	const int index2 = calculator.getIndex(sizeX.y_ - 1, sizeZ.y_ - 1);
	const auto center1 = complexData[index1].getCenter();
	const auto center2 = complexData[index2].getCenter();
	const auto newCenter = (center1 + center2) / 2;

	return newCenter;
}

Urho3D::IntVector2 MainGrid::getBucketCords(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const {
	return {calculator.getIndex(pos.x_), calculator.getIndex(pos.y_)};
}

void MainGrid::updateNeighbors(const int current) const {
	for (unsigned char i = 0; i < 8; ++i) {
		const int nI = current + closeIndexes[1][i]; //TODO bug przy skrajach bedzie całkiem inne indexy updejtowac :O
		if (calculator.validIndex(nI)) {
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
	return (complexData[current].getCenter() - complexData[next].getCenter()).Length();
}

int MainGrid::getCloseIndex(int center, int i) const {
	return center + closeIndexes[1][i];
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
