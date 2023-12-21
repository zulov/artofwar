#include "MainGrid.h"

#include <exprtk/exprtk.hpp>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/IO/Log.h>
#include "Bucket.h"
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "database/db_strcut.h"
#include "debug/DebugLineRepo.h"
#include "levels/LevelCache.h"
#include "math/MathUtils.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "env/CloseIndexes.h"
#include "env/Environment.h"


MainGrid::MainGrid(short resolution, float size, float maxQueryRadius):
	Grid(resolution, size, maxQueryRadius), complexData(new ComplexBucketData[sqResolution]),
	pathFinder(resolution, size) {
	const auto quarter = calculator->getFieldSize() / 4;
	pathFinder.setComplexBucketData(complexData);
	posInBucket = {
		Urho3D::Vector2(quarter, quarter), Urho3D::Vector2(-quarter, -quarter),
		Urho3D::Vector2(quarter, -quarter), Urho3D::Vector2(-quarter, quarter)
	};
	auto ptr = complexData;
	for (int i = 0; i < sqResolution; ++i) {
		ptr->setIndexCloseIndexes(closeIndexes->getBothIndexes(i));
		++ptr;
	}
	DebugLineRepo::init(DebugLineType::MAIN_GRID);
}

MainGrid::~MainGrid() {
	delete[] complexData;
}

void MainGrid::updateNeight(int idx) const {
	auto& data = complexData[idx];
	data.setAllOccupied();
	for (const auto i : closeIndexes->getTabIndexes(data)) {
		data.setNeightFree(i);
	}
}

void MainGrid::prepareGridToFind() const {
	auto lastRow = sqResolution - resolution;
	auto firstColumn = 0;
	auto lastColumn = resolution - 1;
	for (int firstRow = 0; firstRow < resolution; ++firstRow, ++lastRow, firstColumn += resolution, lastColumn +=
	     resolution) {
		updateNeight(firstRow);
		updateNeight(lastRow);
		updateNeight(firstColumn);
		updateNeight(lastColumn);
	}
}

bool MainGrid::validateAdd(const Urho3D::IntVector2& size, const Urho3D::IntVector2 bucketCords,
                           bool isBuilding) const {
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
	if (isBuilding) {
		auto x1 = calculator->getValidLow(sizeX.x_ - 1);
		auto x2 = calculator->getValidHigh(sizeX.y_);

		auto z1 = calculator->getValidLow(sizeZ.x_ - 1);
		auto z2 = calculator->getValidHigh(sizeZ.y_);

		for (int i = x1; i <= x2; ++i) {
			for (int j = z1; j <= z2; ++j) {
				if ((i < sizeX.x_ || i >= sizeX.y_) || (j < sizeZ.x_ || j >= sizeZ.y_)) {
					if (isBuildable(calculator->getNotSafeIndex(i, j))) {
						return true;
					}
				}
			}
		}
		return false;
	}

	return true;
}

Urho3D::Vector2 MainGrid::repulseObstacle(Unit* unit) const {
	const auto index = unit->getMainGridIndex();

	const auto& data = complexData[index];

	Urho3D::Vector2 sum;
	if (index != unit->getIndexToInteract() //TODO ten warunek to chyba nie na ten index
		&& data.isPassable()
		&& data.anyNeightOccupied()) {
		int counter = 0;

		for (auto [i, val] : closeIndexes->getTabIndexesWithValue(data)) {
			if (!data.ifNeightIsFree(i)) {
				sum += calculator->getCenter(index + val);
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

	const auto center = calculator->getCenter(index);
	int i = 0;
	for (auto& phy : buckets[index].getContent()) {
		Unit* unit = (Unit*)phy;
		if (unit->getState() == UnitState::COLLECT || unit->getState() == UnitState::ATTACK) {
			unit->setInCellPos(posInBucket[i] + center);
			i++;
		}
	}
}

unsigned char MainGrid::getRevertCloseIndex(int center, int gridIndex) const {
	const int index = gridIndex - center;
	for (auto [i ,val] : closeIndexes->getTabIndexesWithValue(complexData[center])) {
		if (val == index) {
			return i;
		}
	}

	assert(false);
	Game::getLog()->Write(0, "closeIndex miscalculate");
	return 0;
}

void MainGrid::addDeploy(Building* building) const {
	const auto optDeployIndex = building->getDeploy();

	if (optDeployIndex.has_value()) {
		complexData[optDeployIndex.value()].setDeploy();
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
	const auto centerCord = calculator->getCords(center);
	distThreshold *= distThreshold;
	for (const auto index : indexes) {
		if (calculator->getSqDistance(centerCord, index) < distThreshold) {
			return true;
		}
	}
	return false;
}

std::vector<int> MainGrid::getIndexesInRange(const Urho3D::Vector3& center, float range) const {
	std::vector<int> allIndexes;
	const auto centerIdx = calculator->indexFromPosition(center);

	const auto levels = levelCache->get(range, centerIdx);

	for (const auto idx : *levels) {
		allIndexes.push_back(centerIdx + idx);
	}

	return allIndexes;
}

void MainGrid::addResBonuses(std::vector<Building*>& resBuildings) const {
	std::vector<int> changedIndexes; //TODO perf do it lazy not for all
	for (const auto building : resBuildings) {
		addResourceBonuses(building, changedIndexes);
	}

	std::vector<ResourceEntity*> withBonus;
	withBonus.reserve(changedIndexes.size());
	for (const auto changedIndex : changedIndexes) {
		Static* obj = complexData[changedIndex].getStatic();
		if (obj && obj->getType() == ObjectType::RESOURCE) {
			withBonus.push_back(dynamic_cast<ResourceEntity*>(obj));
		}
	}
	//withBonus usunac duplikwaty
	for (auto withBonu : withBonus) {
		for (char player = 0; player < MAX_PLAYERS; ++player) {
			withBonu->setBonus(player, getBonuses(player, withBonu));
		}
	}
}

void MainGrid::reAddBonuses(std::vector<Building*>& resBuildings, std::vector<ResourceEntity*>* resources) const {
	for (int i = 0; i < sqResolution; ++i) {
		complexData[i].resetResBonuses();
	}
	for (const auto resource : *resources) {
		resource->resetBonus();
	}
	addResBonuses(resBuildings);
}

void MainGrid::addResourceBonuses(Building* building, std::vector<int>& changedIndexes) const {
	const auto [dbBuilding, level] = building->getData();

	if (dbBuilding->typeResourceAny) {
		std::vector<int> indexes;

		for (const int cell : building->getOccupiedCells()) {
			const auto levels = levelCache->get(level->resourceRange, cell);

			for (const auto idx : *levels) {
				indexes.push_back(cell + idx);
			}
		}
		std::ranges::sort(indexes);
		indexes.erase(std::ranges::unique(indexes).begin(), indexes.end());
		std::ranges::copy(indexes, std::back_inserter(changedIndexes));

		for (const int index : indexes) {
			complexData[index].setResBonuses(building->getPlayer(), dbBuilding->resourceTypes, level->collect);
		}
	}
}

float MainGrid::getBonuses(char player, const ResourceEntity* resource) const {
	float best = .0f;
	for (const int cell : resource->getOccupiedCells()) {
		//TODO perf bonus zapisac w resource
		const float val = complexData[cell].getResBonus(player, resource->getId());
		if (val > best) {
			best = val;
		}
	}
	return best;
}

bool MainGrid::isInLocalArea(const int center, int indexOfAim) const {
	return closeIndexes->isInLocalArea(center, indexOfAim);
}

bool MainGrid::isInLocal1and2Area(int center, int indexOfAim) const {
	return closeIndexes->isInLocalArea(center, indexOfAim) || closeIndexes->isInLocalLv2Area(center, indexOfAim);
}

bool MainGrid::isPassable(int inx) const {
	return calculator->isValidIndex(inx) && complexData[inx].isPassable();
}

bool MainGrid::isBuildable(int inx) const {
	return calculator->isValidIndex(inx) && complexData[inx].isBuildable();
}

int MainGrid::closestPassableCell(int posIndex) const {
	//TODO improved pathFinder getPassableEnd
	const auto& data = complexData[posIndex];
	for (auto i : closeIndexes->getLv1(data)) {
		const auto idx = i + posIndex;
		if (complexData[idx].isPassable()) {
			return idx;
		}
	}
	for (auto i : closeIndexes->getLv2(data)) {
		const auto idx = i + posIndex;
		if (complexData[idx].isPassable()) {
			return idx;
		}
	}
	return posIndex; //TODO to zwrócic optional empty
}

void MainGrid::addStatic(Static* object, bool bulkAdd) {
	for (const auto index : object->getOccupiedCells()) {
		complexData[index].setStatic(object);
	}
	if (!bulkAdd) {
		refreshStatic(object->getAllCells());
	}
}

void MainGrid::refreshAllStatic(const std::span<int> allChanged) {
	std::vector<int> toRefresh;
	toRefresh.reserve(9);

	for (const auto index : allChanged) {
		auto& data = complexData[index];
		updateNeighbors(data, index);
		if (data.isPassable()) {
			data.setGradient(data.allNeightFree() ? -1 : 0);
		} else {
			toRefresh.push_back(index);
		}
	}

	refreshAllGradient(toRefresh);
}

void MainGrid::refreshStatic(const std::span<int> changed) {
	std::vector<int> toRefresh;
	toRefresh.reserve(9);

	for (const auto index : changed) {
		auto& data = complexData[index];
		updateNeighbors(data, index);
		if (data.isPassable()) {
			data.setGradient(data.allNeightFree() ? -1 : 0);
		} else {
			toRefresh.push_back(index);
		}
	}

	refreshGradient(toRefresh);
}

const std::vector<std::pair<unsigned char, short>>& MainGrid::getCloseTabIndexesWithValue(int center) const {
	return closeIndexes->getTabIndexesWithValue(complexData[center]);
}

const std::vector<short>& MainGrid::getCloseIndexes(int center) const {
	return closeIndexes->getLv1(complexData[center]);
}

void MainGrid::refreshGradient(const std::vector<int>& notPassables) const {
	std::vector<int> toRefresh = notPassables;
	std::vector<int> toDo = notPassables;
	std::vector<int> newAdded;
	while (!toDo.empty()) {
		for (const auto index : toDo) {
			auto& data = complexData[index];
			data.setGradient(1024);
			for (short value : closeIndexes->getLv1(data)) {
				auto neightIdx = value + index;
				auto& neightData = complexData[neightIdx];
				if (!neightData.isPassable() && neightData.allNeightOccupied() && neightData.getGradient() < 1024) {
					toRefresh.push_back(neightIdx);
					newAdded.push_back(neightIdx);
				}
			}
		}
		toDo = newAdded;
		newAdded.clear();
	}
	refreshAllGradient(toRefresh);
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
	refreshGradientRemoveStatic(object->getOccupiedCells());
}

void MainGrid::refreshAllGradient(std::vector<int>& toRefresh) const {
	std::vector<int> nextToRefresh;

	short level = 0;
	for (int idx : toRefresh) {
		auto& current = complexData[idx];
		if (current.allNeightOccupied()) {
			nextToRefresh.push_back(idx);
		} else {
			current.setGradient(level);
		}
	}
	while (!nextToRefresh.empty()) {
		createGradient(nextToRefresh, level);
		level++;
	}
}

void MainGrid::refreshGradientRemoveStatic(std::span<int> toRefresh) const {
	//tu wymyslic
}

void MainGrid::createGradient(std::vector<int>& toRefresh, short level) const {
	std::vector<int> toRefresh2;
	for (int current : toRefresh) {
		auto& currentCell = complexData[current];
		for (const auto indexVal : closeIndexes->getLv1(currentCell)) {
			auto& neight = complexData[current + indexVal];
			if (neight.getGradient() == level) {
				currentCell.setGradient(level + 1);
				break;
			}
		}
		if (currentCell.getGradient() == -1) {
			toRefresh2.push_back(current);
		}
	}
	toRefresh = toRefresh2;
}

std::optional<Urho3D::Vector2> MainGrid::getDirectionFrom(int index, const Urho3D::Vector3& position) const {
	auto& data = complexData[index];

	if (!data.isPassable()) {
		int escapeBucket = -1;
		if (data.anyNeightFree()) {
			float dist = 999999;

			for (auto [i, val] : closeIndexes->getTabIndexesWithValue(data)) {
				if (data.ifNeightIsFree(i)) {
					const int ni = index + val;

					float newDist = calculator->getSqDistance(ni, index);
					if (newDist < dist) {
						dist = newDist;
						escapeBucket = ni;
					}
				}
			}
		} else {
			auto currentGradient = data.getGradient();
			for (short i : closeIndexes->getLv1(data)) {
				//TODO brac lepszy nic pierwszy z brzegu
				auto neightIndex = i + index;
				if (currentGradient < complexData[neightIndex].getGradient()) {
					escapeBucket = neightIndex;
					break;
				}
			}
		}
		if (escapeBucket == -1) {
			assert(false);
			return {};
		}
		auto direction = dirTo(position, calculator->getCenter(escapeBucket));

		direction.Normalize();
		return direction;
	}
	return {};
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

void MainGrid::updateNeighbors(ComplexBucketData& data, const int dataIndex) const {
	data.setAllOccupied();
	for (const auto& p : closeIndexes->getTabIndexesWithValue(data)) {
		if (complexData[dataIndex + p.second].isPassable()) {
			data.setNeightFree(p.first);
		}
	}
}

const std::vector<int>* MainGrid::findPath(int startIdx, int endIdx, int limit) {
	return pathFinder.findPath(startIdx, endIdx, limit);
}

const std::vector<int>* MainGrid::findPath(int startIdx, const std::vector<int>& endIdxs, int limit, bool closeEnough) {
	return pathFinder.findPath(startIdx, endIdxs, limit, closeEnough);
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
