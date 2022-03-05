#include "MainGrid.h"

#include <exprtk/exprtk.hpp>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/IO/Log.h>
#include "Bucket.h"
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "debug/DebugLineRepo.h"
#include "levels/LevelCache.h"
#include "math/MathUtils.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "env/CloseIndexes.h"
#include "env/Environment.h"


MainGrid::MainGrid(short resolution, float size, float maxQueryRadius):
	Grid(resolution, size, true, maxQueryRadius), complexData(new ComplexBucketData[sqResolution]),
	pathFinder(resolution, size) {
	const auto quarter = calculator->getFieldSize() / 4;
	pathFinder.setComplexBucketData(complexData);
	posInBucket = {
		Urho3D::Vector2(quarter, quarter), Urho3D::Vector2(-quarter, -quarter),
		Urho3D::Vector2(quarter, -quarter), Urho3D::Vector2(-quarter, quarter)
	};

	DebugLineRepo::init(DebugLineType::MAIN_GRID);
}

MainGrid::~MainGrid() {
	delete[] complexData;
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
	auto index = unit->getMainGridIndex();

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

void MainGrid::invalidatePathCache() {
	pathFinder.invalidateCache();
}

bool MainGrid::cellInState(int index, CellState state) const {
	return complexData[index].getType() == state;
}

void MainGrid::updateCell(int index, char val, CellState cellState) const {
	assert(calculator->isValidIndex(index));
	complexData[index].updateSize(val, cellState);
}

Urho3D::Vector2 MainGrid::getPositionInBucket(Unit* unit) const {
	const auto index = unit->getMainGridIndex();

	const auto center = calculator->getCenter(index);

	auto& content = buckets[index].getContent();
	if (content.size() == 1) {
		return center;
	}

	const auto pos = std::ranges::find(content, unit);
	const auto idx = pos - content.begin();
	assert(idx >= 0 && idx <= 4);
	return posInBucket[idx] + center;
}

unsigned char MainGrid::getRevertCloseIndex(int center, int gridIndex) const {
	int index = gridIndex - center;
	for (auto i : closeIndexes->getTabIndexes(center)) {
		if (closeIndexes->getIndexAt(i) == index) {
			return i;
		}
	}
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
	case GridDebugType::CELLS_TYPE: {
		for (int i = 0; i < sqResolution; ++i) {
			auto center = calculator->getCenter(i);
			std::tuple<bool, Urho3D::Color> info = Game::getColorPaletteRepo()->
				getInfoForGrid(complexData[i].getType());

			if (std::get<0>(info)) {
				const auto v = calculator->getFieldSize() / 2.3f;
				const auto a = getVertex(center, Urho3D::Vector2(-v, v));
				const auto b = getVertex(center, Urho3D::Vector2(v, -v));
				const auto c = getVertex(center, Urho3D::Vector2(v, v));
				const auto d = getVertex(center, Urho3D::Vector2(-v, -v));

				// DebugLineRepo::drawTriangle(DebugLineType::MAIN_GRID, {center.x_, 10, center.y_},
				//                         {center.x_, 20, center.y_}, std::get<1>(info));

				DebugLineRepo::drawTriangle(DebugLineType::MAIN_GRID, a, c, b, std::get<1>(info));
				DebugLineRepo::drawTriangle(DebugLineType::MAIN_GRID, b, d, a, std::get<1>(info));
			}
		}
	}
	break;
	default: ;
	}

	DebugLineRepo::commit(DebugLineType::MAIN_GRID);
}

Urho3D::Vector3 MainGrid::getVertex(const Urho3D::Vector2 center, Urho3D::Vector2 vertex) const {
	auto result = Game::getEnvironment()->getPosWithHeightAt(center.x_ + vertex.x_, center.y_ + vertex.y_);
	result.y_ += 1.f;
	return result;
}

float MainGrid::getFieldSize() const {
	return calculator->getFieldSize();
}

void MainGrid::drawAll() {
	auto image = new Urho3D::Image(Game::getContext());
	image->SetSize(calculator->getResolution(), calculator->getResolution(), 4);

	Urho3D::String prefix = Urho3D::String(counter) + "_";

	drawComplex(image, prefix);

	++counter;

	delete image;
}

bool MainGrid::cellIsCollectable(int index) const {
	return complexData[index].cellIsCollectable();
}

bool MainGrid::cellIsAttackable(int index) const {
	return complexData[index].cellIsAttackable();
}

bool MainGrid::anyCloseEnough(std::vector<int> const& indexes, int center, float distThreshold) const {
	//TODO perf dac square anie sqroot
	const auto centerCord = calculator->getIndexes(center);
	for (const auto index : indexes) {
		if (calculator->getDistance(centerCord, index) < distThreshold) {
			return true;
		}
	}
	return false;
}

std::vector<int> MainGrid::getIndexesInRange(const Urho3D::Vector3& center, float range) const {
	std::vector<int> allIndexes;
	const auto centerIdx = calculator->indexFromPosition(center);

	const auto centerCords = calculator->getIndexes(centerIdx);

	auto const [indexes, cords] = levelCache->getBoth(range);
	auto ptrIdx = indexes->begin();
	for (const auto& shiftCords : *cords) {
		if (calculator->isValidIndex(shiftCords.x_ + centerCords.x_, shiftCords.y_ + centerCords.y_)) {
			allIndexes.push_back(*ptrIdx + centerIdx);
		}
		++ptrIdx;
	}
	return allIndexes;
}

bool MainGrid::isInLocalArea(const int center, int indexOfAim) const {
	return closeIndexes->isInLocalArea(center, indexOfAim);
}

bool MainGrid::isInLocal1and2Area(int center, int indexOfAim) const {
	return closeIndexes->isInLocalArea(center, indexOfAim) || closeIndexes->isInLocal2Area(center, indexOfAim);
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
	return posIndex; //TODO to zwrócic optional empty
}

void MainGrid::addStatic(Static* object) {
	const auto bucketPos = calculator->getIndexes(object->getMainGridIndex());

	for (const auto index : object->getOccupiedCells()) {
		complexData[index].setStatic(object);
	}

	std::vector<int> toRefresh;
	toRefresh.reserve(9);

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
			auto& data = complexData[index];
			updateNeighbors(data, index);
			if (data.isPassable()) {
				data.setEscapeThrough(-1);
			} else {
				toRefresh.push_back(index);
			}
			++index;
		}
	}
	pathFinder.refreshWayOut(toRefresh);
}

void MainGrid::removeStatic(Static* object) const {
	for (const auto index : object->getOccupiedCells()) {
		auto& data = complexData[index];
		data.clear();
		updateNeighbors(data, index);
	}
	for (const auto index : object->getSurroundCells()) {
		updateNeighbors(complexData[index], index);
	}
	//TODO BUG remove deploy??
}

std::optional<Urho3D::Vector2> MainGrid::getDirectionFrom(int index, const Urho3D::Vector3& position) const {
	//assert(unit->getMainGridIndex()==index);//TODO perf zamienic jezeli ok
	auto& data = complexData[index];

	if (!data.isPassable()) {
		int escapeBucket; //=-1
		//auto& neights = complexData[index].getNeightbours();
		if (!data.allNeightOccupied()) {
			float dist = 999999;
			const auto center = calculator->getCenter(index);
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

void MainGrid::updateNeighbors(ComplexBucketData& data, const int current) const {
	for (auto i : closeIndexes->getTabIndexes(current)) {
		const int nI = current + closeIndexes->getIndexAt(i);
		if (complexData[nI].isPassable()) {
			data.setNeightFree(i);
		} else {
			data.setNeightOccupied(i);
		}
	}
}

float inline MainGrid::cost(const Urho3D::IntVector2& centerParams, int next) const {
	return calculator->getDistance(centerParams, next);
}

const std::vector<int>* MainGrid::findPath(int startIdx, int endIdx, int limit) {
	return pathFinder.findPath(startIdx, endIdx, limit);
}

const std::vector<int>* MainGrid::findPath(int startIdx, const std::vector<int>& endIdxs, int limit) {
	return pathFinder.findPath(startIdx, endIdxs, limit);
}

void MainGrid::drawComplex(Urho3D::Image* image, const Urho3D::String prefix) const {
	for (short y = 0; y != calculator->getResolution(); ++y) {
		for (short x = 0; x != calculator->getResolution(); ++x) {
			const int index = calculator->getIndex(x, y);

			image->SetPixel(x, calculator->getResolution() - y - 1,
			                std::get<1>(Game::getColorPaletteRepo()->getInfoForGrid(complexData[index].getType())));
		}
	}

	image->SavePNG("result/images/complexData/type_" + prefix + ".png");
}
