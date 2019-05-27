#include "MainGrid.h"
#include "Bucket.h"
#include "Game.h"
#include "MathUtils.h"
#include "Urho3D/Resource/Image.h"
#include "objects/static/Static.h"
#include "objects/unit/Unit.h"
#include "simulation/env/ContentInfo.h"
#include <array>
#include <unordered_set>
#include "objects/building/Building.h"
#include "DebugLineRepo.h"


MainGrid::MainGrid(const short _resolution, const float _size): Grid(_resolution, _size),
                                                                closeIndex({
	                                                                -resolution - 1, -resolution, -resolution + 1, -1,
	                                                                0, 1, resolution - 1, resolution, resolution + 1
                                                                }) {
	short posX = 0;
	short posZ = 0;

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

	ci = new content_info();
	switchDebugGrid();
	int a = sizeof(ComplexBucketData);

	int b = 3;
}

MainGrid::~MainGrid() {
	delete[] complexData;
	delete pathConstructor;
	delete ci;
}

void MainGrid::prepareGridToFind() {
	for (int i = 0; i < resolution * resolution; ++i) {
		updateNeighbors(i);
	}
	pathConstructor->prepareGridToFind();
}


bool MainGrid::validateAdd(Static* object) {
	const auto pos = object->getPosition();
	return validateAdd(object->getGridSize(), Urho3D::Vector2(pos->x_, pos->z_));
}

bool MainGrid::validateAdd(const Urho3D::IntVector2& size, Urho3D::Vector2& pos) {
	const auto sizeX = calculateSize(size.x_, getIndex(pos.x_));
	const auto sizeZ = calculateSize(size.y_, getIndex(pos.y_));

	for (int i = sizeX.x_; i < sizeX.y_; ++i) {
		for (int j = sizeZ.x_; j < sizeZ.y_; ++j) {
			const int index = getIndex(i, j);
			if (!(inRange(index) && complexData[index].isUnit())) {
				return false;
			}
		}
	}

	return true;
}

content_info* MainGrid::getContentInfo(const Urho3D::Vector2& from, const Urho3D::Vector2& to, bool checks[],
                                       int activePlayer) {
	const auto posBeginX = getIndex(from.x_);
	const auto posBeginZ = getIndex(from.y_);
	const auto posEndX = getIndex(to.x_);
	const auto posEndZ = getIndex(to.y_);

	const auto iMin = Urho3D::Min(posBeginX, posEndX);
	const auto iMax = Urho3D::Max(posBeginX, posEndX);
	const auto jMin = Urho3D::Min(posBeginZ, posEndZ);
	const auto jMax = Urho3D::Max(posBeginZ, posEndZ);

	ci->reset();

	for (short i = iMin; i < iMax; ++i) {
		for (short j = jMin; j < jMax; ++j) {
			const int index = i * resolution + j;
			updateInfo(index, ci, checks, activePlayer);
		}
	}
	return ci;
}

Urho3D::Vector2 MainGrid::repulseObstacle(Unit* unit) {
	auto index = indexFromPosition(unit->getPosition());

	Urho3D::Vector2 sum;
	if (index != unit->getIndexToInteract()
		&& complexData[index].isUnit()
		&& complexData[index].allNeightOccupied()) {
		int counter = 0;
		for (int i = 0; i < 8; ++i) {
			if (!complexData[index].ifNeightIsFree(i)) {
				sum += complexData[index + closeIndex[i]].getCenter();
				++counter;
			}
		}
		sum /= counter;
		sum -= Urho3D::Vector2(unit->getPosition()->x_, unit->getPosition()->z_);
	}
	return -sum;
}

void MainGrid::invalidateCache() {
	pathConstructor->invalidateCache();
}

void MainGrid::updateSurround(Static* object) {
	if (object->isAlive()) {
		std::unordered_set<int> indexes;
		for (auto index : object->getOccupiedCells()) {
			for (auto inIndex : closeIndex) {
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
		if (closeIndex[i] == index) {
			return i;
		}
	}
	Game::getLog()->Write(0, "closeIndex miscalculate");
	return 0;
}

void MainGrid::addDeploy(Building* building) {
	auto optDeployIndex = building->getDeploy();

	if (optDeployIndex.has_value()) {
		complexData[optDeployIndex.value()].setDeploy(building);
	}
}

void MainGrid::removeDeploy(Building* building) {
	complexData[building->getDeploy().value()].removeDeploy();
}

void MainGrid::updateInfo(int index, content_info* ci, bool* checks, int activePlayer) {
	switch (complexData[index].getType()) {
	case CellState::NONE:
	case CellState::COLLECT:
	case CellState::EMPTY:
	case CellState::ATTACK:
	{
		if (checks[3] || checks[4]) {
			const bool hasInc = buckets[index].incUnitsPerPlayer(ci, activePlayer, checks);
			if (hasInc) {
				ci->hasUnit = true;
			}
		}
	}
		break;
	case CellState::RESOURCE:
		if (checks[1]) {
			ci->hasResource = true;
			ci->resourceNumber[complexData[index].getAdditionalInfo()]++;
		}
		break;
	case CellState::BUILDING:
		if (checks[2]) {
			ci->hasBuilding = true;
			ci->buildingNumberPerPlayer[complexData[index].getAdditionalInfo()]++;
		}
		break;
	default: ;
	}
}

void MainGrid::drawDebug() {
	DebugLineRepo::init(DebugLineType::MAIN_GRID);
	DebugLineRepo::clear(DebugLineType::MAIN_GRID);

	DebugLineRepo::beginGeometry(DebugLineType::MAIN_GRID);

	switch (debugType) {
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
				                        Urho3D::Vector3(center.x_, 15, center.y_), std::get<1>(info));
			}
		}
	}
	break;
	default: ;
	}

	DebugLineRepo::commit(DebugLineType::MAIN_GRID);
}

void MainGrid::switchDebugGrid() {
	switch (debugType) {
	case GridDebugType::NONE:
		debugType = GridDebugType::GRID;
		break;
	case GridDebugType::GRID:
		debugType = GridDebugType::CELLS_TYPE;
		break;
	case GridDebugType::CELLS_TYPE:
		debugType = GridDebugType::NONE;
		break;
	default: ;
	}
	drawDebug();
}

void MainGrid::addStatic(Static* object) {
	if (validateAdd(object)) {
		const auto bucketPos = getCords(object->getMainCell());

		object->setMainCell(getIndex(bucketPos.x_, bucketPos.y_));

		for (auto index : object->getOccupiedCells()) {
			complexData[index].setStatic(object);
		}

		std::vector<int> toRefresh;

		const auto size = object->getGridSize();
		const auto sizeX = calculateSize(size.x_, bucketPos.x_);
		const auto sizeZ = calculateSize(size.y_, bucketPos.y_);

		for (int i = sizeX.x_ - 1; i < sizeX.y_ + 1; ++i) {
			for (int j = sizeZ.x_ - 1; j < sizeZ.y_ + 1; ++j) {
				const int index = getIndex(i, j);
				updateNeighbors(index);
				if (!complexData[index].isUnit()) {
					toRefresh.push_back(index);
				} else {
					complexData[index].setEscapeThrought(-1);
				}
			}
		}
		pathConstructor->refreshWayOut(toRefresh);
	}
	updateSurround(object);
}

void MainGrid::removeStatic(Static* object) {
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

Urho3D::Vector2* MainGrid::getDirectionFrom(Urho3D::Vector3* position) {
	int index = indexFromPosition(position);
	if (!complexData[index].isUnit()) {
		int escapeBucket;//=-1
		//auto& neights = complexData[index].getNeightbours();
		if (!complexData[index].allNeightOccupied()) {
			float dist = 999999;
			for (int i = 0; i < 8; ++i) {
				if (complexData[index].ifNeightIsFree(i)) {
					int ni = index + closeIndex[i];
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

Urho3D::Vector2 MainGrid::getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) {
	//TODO tu mozna to sporo zoptymalizowac ale pewnie nie ma potrzeby
	const short posX = getIndex(pos.x_);
	const short posZ = getIndex(pos.y_);

	const auto sizeX = calculateSize(size.x_, posX);
	const auto sizeZ = calculateSize(size.y_, posZ);

	const int index1 = getIndex(sizeX.x_, sizeZ.x_);
	const int index2 = getIndex(sizeX.y_ - 1, sizeZ.y_ - 1);
	const auto center1 = complexData[index1].getCenter();
	const auto center2 = complexData[index2].getCenter();
	const auto newCenter = (center1 + center2) / 2;

	return newCenter;
}

Urho3D::IntVector2 MainGrid::getBucketCords(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const {
	return {getIndex(pos.x_), getIndex(pos.y_)};
}

void MainGrid::updateNeighbors(const int current) const {
	for (unsigned char i = 0; i < 8; ++i) {
		const int nI = current + closeIndex[i]; //TODO bug przy skrajach bedzie całkiem inne indexy updejtowac :O
		if (inSide(nI)) {
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
	return center + closeIndex[i];
}

// std::vector<int>* MainGrid::findPath(Urho3D::IntVector2& startV, Urho3D::IntVector2& goalV) {
// 	int start = getIndex(startV.x_, startV.y_);
// 	int goal = getIndex(goalV.x_, goalV.y_);
// 	float min = cost(start, goal);
// 	return pathConstructor->findPath(start, goal, min, min * 2);
// }

std::vector<int>* MainGrid::findPath(int startIdx, const Urho3D::Vector2& aim) {
	return pathConstructor->findPath(startIdx, aim);
}

void MainGrid::drawMap(Urho3D::Image* image) {
	const auto data = (uint32_t*)image->GetData();
	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			const int index = getIndex(x, y);
			const int idR = getIndex(resolution - y - 1, x);
			if (complexData[index].isUnit()) {
				*(data + idR) = 0xFFFFFFFF;
			} else {
				*(data + idR) = 0xFF000000;
			}
		}
	}
}
