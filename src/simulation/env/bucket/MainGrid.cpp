#include "MainGrid.h"

#include <exprtk/exprtk.hpp>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/IO/Log.h>
#include "Bucket.h"
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "debug/DebugLineRepo.h"
#include "math/MathUtils.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "simulation/env/CloseIndexes.h"


MainGrid::MainGrid(short resolution, float size, float maxQueryRadius): Grid(resolution, size, maxQueryRadius) {
	complexData = new ComplexBucketData[sqResolution];
	auto quater = calculator->getFieldSize() / 4;
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
	for (int current = 0; current < sqResolution; ++current) {
		auto& data = complexData[current];
		auto centerParams = calculator->getIndexes(current);

		for (auto i : closeIndexes->getTabIndexes(current)) {
			data.setNeightFree(i);
			const int nI = current + closeIndexes->getIndexAt(i);
			data.setCost(i, cost(centerParams, nI));
		}
	}
	pathConstructor->prepareGridToFind();
}

bool MainGrid::validateAdd(const Urho3D::IntVector2& size, Urho3D::Vector2& pos) const {
	return validateAdd(size, {calculator->getIndex(pos.x_), calculator->getIndex(pos.y_)});
}

bool MainGrid::validateAdd(const Urho3D::IntVector2& size, const Urho3D::IntVector2 bucketCords) const {
	const auto sizeX = calculateSize(size.x_, bucketCords.x_);
	const auto sizeZ = calculateSize(size.y_, bucketCords.y_);
	//WARN to troche złe użycie tego ale działa
	if (!calculator->isValidIndex(sizeX.x_, sizeX.y_)
		|| !calculator->isValidIndex(sizeZ.x_, sizeZ.y_)) {
		return false;
	}
	for (int i = sizeX.x_; i < sizeX.y_; ++i) {
		int index = calculator->getNotSafeIndex(i, sizeZ.x_);
		for (int j = sizeZ.x_; j < sizeZ.y_; ++j) {
			if (!isBuildable(index)) {
				return false;
			}
			++index;
		}
	}
	//TODO bug validate deploy przynajmniej jeden

	return true;
}

Urho3D::Vector2 MainGrid::repulseObstacle(Unit* unit) const {
	auto index = unit->getMainBucketIndex();

	auto& data = complexData[index];

	Urho3D::Vector2 sum;
	if (index != unit->getIndexToInteract()
		&& data.isPassable()
		&& data.allNeightOccupied()) {
		int counter = 0;

		for (auto i : closeIndexes->getTabIndexes(index)) {
			if (!data.ifNeightIsFree(i)) {
				sum += calculator->getCenter(index + closeIndexes->getIndexAt(i));
				++counter;
			}
		}
		sum /= counter;
		sum -= Urho3D::Vector2(unit->getPosition().x_, unit->getPosition().z_);
	}
	return -sum;
}

void MainGrid::invalidatePathCache() const {
	pathConstructor->invalidateCache();
}

void MainGrid::updateSurround(Static* object) const {
	if (object->isAlive()) {
		const auto& occu = object->getOccupiedCells();
		std::vector<int> indexes;
		indexes.reserve(occu.size() * 8);

		for (auto index : occu) {
			for (auto inIndex : closeIndexes->get(index)) {
				indexes.emplace_back(index + inIndex);
			}
		}
		std::ranges::sort(indexes);
		indexes.erase(std::unique(indexes.begin(), indexes.end()), indexes.end());
		indexes.erase(
			std::ranges::remove_if(indexes,
			                       [&](auto x) {
				                       return std::find(occu.begin(), occu.end(), x) != occu.end();
			                       }).begin(),
			indexes.end());

		object->setSurroundCells(indexes);
	}
}

bool MainGrid::cellInState(int index, CellState state) const {
	return complexData[index].getType() == state;
}

void MainGrid::updateCell(int index, char val, CellState cellState) const {
	complexData[index].updateSize(val, cellState);
}

Urho3D::Vector2 MainGrid::getPositionInBucket(Unit* unit) {
	const auto index = unit->getMainCell();
	char ordinal = -1;
	char max = 0;
	const auto state = unit->getState();
	for (auto physical : buckets[index].getContent()) {
		if (physical == unit && ordinal < 0) {
			ordinal = max;
		}
		if (static_cast<Unit*>(physical)->getState() == state) {
			++max;
		}
	}
	auto center = calculator->getCenter(index);
	switch (max) {
	case 1:
		return center;
	case 2:
		return posInBucket2[ordinal] + center;
	case 3:
		return posInBucket3[ordinal] + center;
	case 4:
		return posInBucket4[ordinal] + center;
	}
}

unsigned char MainGrid::getRevertCloseIndex(int center, int gridIndex) const {
	int index = gridIndex - center;
	for (auto i : closeIndexes->getTabIndexes(center)) {
		if (closeIndexes->getIndexAt(i) == index) {
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
	complexData[building->getDeploy().value()].clear();
}

CellState MainGrid::getCellAt(float x, float z) const {
	auto index = calculator->indexFromPosition(x, z);
	return complexData[index].getType();
}

int MainGrid::getAdditionalInfoAt(float x, float z) const {
	auto index = calculator->indexFromPosition(x, z);

	return complexData[index].getAdditionalInfo();
}

void MainGrid::drawDebug(GridDebugType type) const {
	DebugLineRepo::clear(DebugLineType::MAIN_GRID);
	DebugLineRepo::beginGeometry(DebugLineType::MAIN_GRID);

	switch (type) {
	case GridDebugType::NONE:
		break;
	case GridDebugType::GRID: {
		float size = calculator->getSize();
		float value = -size / 2;
		for (int i = 0; i < resolution; ++i) {
			DebugLineRepo::drawLine(DebugLineType::MAIN_GRID, {-size / 2, 10, value},
			                        {size / 2, 10, value}, Urho3D::Color::CYAN);
			DebugLineRepo::drawLine(DebugLineType::MAIN_GRID, {value, 10, -size / 2},
			                        {value, 10, size / 2}, Urho3D::Color::CYAN);
			value += calculator->getFieldSize();
		}
	}
	break;
	case GridDebugType::CELLS_TYPE: {
		for (int i = 0; i < sqResolution; ++i) {
			auto center = calculator->getCenter(i);
			std::tuple<bool, Urho3D::Color> info = Game::getColorPaletteRepo()->
				getInfoForGrid(complexData[i].getType());

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
	return calculator->getFieldSize();
}

void MainGrid::drawAll() {
	auto image = new Urho3D::Image(Game::getContext());
	image->SetSize(resolution, resolution, 4);

	Urho3D::String prefix = Urho3D::String(counter) + "_";

	drawComplex(image, prefix);

	++counter;

	delete image;
}

bool MainGrid::cellIsCollectable(int index) const {
	return complexData[index].cellIsCollectable();
}

bool MainGrid::anyCloseEnough(std::vector<int> const& indexes, int center, float distThreshold) const {
	//TODO perf dac square
	//TODO perf center obliczyc raz
	for (auto index : indexes) {
		if (calculator->getDistance(index, center) < distThreshold) {
			return true;
		}
	}
	return false;
}

bool MainGrid::isInLocalArea(const int center, int indexOfAim) const {
	if (center == indexOfAim) { return true; }
	auto diff = indexOfAim - center; //center + value == indexOfAim
	for (auto value : closeIndexes->get(indexOfAim)) {
		if (diff == value) {
			return true;
		}
	}
	return false;
}

bool MainGrid::isPassable(int inx) const {
	return calculator->isValidIndex(inx) && complexData[inx].isPassable();
}

bool MainGrid::isBuildable(int inx) const {
	return calculator->isValidIndex(inx) && complexData[inx].isBuildable();
}

int MainGrid::closestPassableCell(int posIndex) const {
	//TODO improved pathFinder getPassableEnd
	for (auto i : closeIndexes->get(posIndex)) {
		if (complexData[i + posIndex].isPassable()) {
			return i + posIndex;
		}
	}
	for (auto i : closeIndexes->getSecond(posIndex)) {
		if (complexData[i + posIndex].isPassable()) {
			return i + posIndex;
		}
	}
	return posIndex;
}

void MainGrid::addStatic(Static* object) const {
	const auto bucketPos = getCords(object->getMainCell());

	for (auto index : object->getOccupiedCells()) {
		complexData[index].setStatic(object);
	}

	std::vector<int> toRefresh;
	toRefresh.reserve(4);

	const auto size = object->getGridSize();
	const auto sizeX = calculateSize(size.x_, bucketPos.x_);
	const auto sizeZ = calculateSize(size.y_, bucketPos.y_);

	const auto iMin = calculator->getValid(sizeX.x_ - 1);
	const auto iMax = calculator->getValid(sizeX.y_ + 1);

	const auto jMin = calculator->getValid(sizeZ.x_ - 1);
	const auto jMax = calculator->getValid(sizeZ.y_ + 1);

	for (int i = iMin; i < iMax; ++i) {
		int index = calculator->getNotSafeIndex(i, jMin);
		for (int j = jMin; j < jMax; ++j) {
			updateNeighbors(index);
			if (complexData[index].isPassable()) {
				complexData[index].setEscapeThrough(-1);
			} else {
				toRefresh.push_back(index);
			}
			++index;
		}
	}
	pathConstructor->refreshWayOut(toRefresh);

	updateSurround(object);
}

void MainGrid::removeStatic(Static* object) const {
	for (auto index : object->getOccupiedCells()) {
		complexData[index].clear();
		updateNeighbors(index);
	}
	for (auto index : object->getSurroundCells()) {
		updateNeighbors(index);
	}
}

std::optional<Urho3D::Vector2> MainGrid::getDirectionFrom(Urho3D::Vector3& position) const {
	int index = calculator->indexFromPosition(position);
	//assert(unit->getMainBucketIndex()==index);//TODO perf zamienic jezeli ok
	auto& data = complexData[index];

	if (!data.isPassable()) {
		int escapeBucket; //=-1
		//auto& neights = complexData[index].getNeightbours();
		if (!data.allNeightOccupied()) {
			float dist = 999999;
			auto center = calculator->getCenter(index);
			for (auto i : closeIndexes->getTabIndexes(index)) {
				if (data.ifNeightIsFree(i)) {
					int ni = index + closeIndexes->getIndexAt(i);
					float newDist = sqDist(calculator->getCenter(ni), center);
					//TODO perf da sie obliczyc oglosc bez obliczania centrów calculator->getDistance
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
			.getDirectionFrom(position, calculator->getCenter(escapeBucket));

		direction.Normalize();
		return direction;
	}
	return {};
}

Urho3D::Vector2 MainGrid::getValidPosition(const Urho3D::IntVector2& size, const Urho3D::Vector2& pos) const {
	return getValidPosition(size, Urho3D::IntVector2(calculator->getIndex(pos.x_), calculator->getIndex(pos.y_)));
}

Urho3D::Vector2 MainGrid::getValidPosition(const Urho3D::IntVector2& size, const Urho3D::IntVector2& cords) const {
	const auto sizeX = calculateSize(size.x_, cords.x_);
	const auto sizeZ = calculateSize(size.y_, cords.y_);

	const int index1 = calculator->getIndex(sizeX.x_, sizeZ.x_);
	const int index2 = calculator->getIndex(sizeX.y_ - 1, sizeZ.y_ - 1);
	const auto center1 = calculator->getCenter(index1);
	const auto center2 = calculator->getCenter(index2);

	return (center1 + center2) / 2;
}

void MainGrid::updateNeighbors(const int current) const {
	//if (calculator->isValidIndex(current)) {
	auto& data = complexData[current];
	for (auto i : closeIndexes->getTabIndexes(current)) {
		const int nI = current + closeIndexes->getIndexAt(i);
		if (complexData[nI].isPassable()) {
			data.setNeightFree(i);
		} else {
			data.setNeightOccupied(i);
		}
	}
	//}
}

float MainGrid::cost(Urho3D::IntVector2& centerParams, int next) const {
	return calculator->getDistance(centerParams, next);
}

std::vector<int>* MainGrid::findPath(int startIdx, const Urho3D::Vector2& aim, int limit) const {
	return pathConstructor->findPath(startIdx, aim, limit);
}

std::vector<int>* MainGrid::findPath(int startIdx, int endIdx, int limit) const {
	return pathConstructor->findPath(startIdx, endIdx, limit);
}

std::vector<int>* MainGrid::findPath(int startIdx, const std::vector<int>& endIdxs, int limit) const {
	return pathConstructor->findPath(startIdx, endIdxs, limit);
}

void MainGrid::drawComplex(Urho3D::Image* image, Urho3D::String prefix) const {
	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			const int index = calculator->getIndex(x, y);

			image->SetPixel(x, resolution - y - 1,
			                std::get<1>(Game::getColorPaletteRepo()->getInfoForGrid(complexData[index].getType())));
		}
	}

	image->SavePNG("result/images/complexData/type_" + prefix + ".png");
}
