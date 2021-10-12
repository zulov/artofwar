#include "Static.h"

#include <magic_enum.hpp>
#include <string>
#include "Game.h"
#include "math/MathUtils.h"
#include "../unit/Unit.h"
#include "database/db_strcut.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Environment.h"
#include "utils/OtherUtils.h"

Static::Static(Urho3D::Vector3& _position, int indexInGrid, bool withNode) : Physical(_position, withNode),
                                                                          state(StaticState::ALIVE),
                                                                          nextState(StaticState::ALIVE) {
	this->indexInMainGrid=indexInGrid;
}

Static::~Static() {
	delete data;
}

void Static::load(dbload_static* dbloadStatic) {
	Physical::load(dbloadStatic);
	this->state = StaticState(dbloadStatic->state);
	this->nextState = StaticState(dbloadStatic->nextState);
}

void Static::setIndexInInfluence(int index) {
	assert(index < std::numeric_limits<unsigned short>::max());
	assert(index >= 0);
	indexInInfluence = index;
}

bool Static::isInCloseRange(int index) const {
	auto cells = getSurroundCells();
	return std::ranges::find(cells, index) != cells.end();
}

void Static::populate() {
	//assert(mainCell==indexInMainGrid);
	std::vector<int> occupiedCells1;

	auto gridSize = getGridSize();
	const auto cordsCell = Game::getEnvironment()->getCords(indexInMainGrid);
	const auto sizeX = calculateSize(gridSize.x_, cordsCell.x_);
	const auto sizeZ = calculateSize(gridSize.y_, cordsCell.y_);
	occupiedCells1.reserve(gridSize.x_ * gridSize.y_);
	for (short i = sizeX.x_; i < sizeX.y_; ++i) {
		for (short j = sizeZ.x_; j < sizeZ.y_; ++j) {
			occupiedCells1.push_back(Game::getEnvironment()->getIndex(i, j));
		}
	}

	std::vector<int> surroundCells1;
	surroundCells1.reserve(occupiedCells1.size() * 8);

	for (auto index : occupiedCells1) {
		for (auto inIndex : Game::getEnvironment()->getCloseIndexs(index)) {
			surroundCells1.emplace_back(index + inIndex);
		}
	}
	std::ranges::sort(surroundCells1);
	surroundCells1.erase(std::unique(surroundCells1.begin(), surroundCells1.end()), surroundCells1.end());
	surroundCells1.erase(
		std::ranges::remove_if(surroundCells1,
		                       [&](auto x) {
			                       return std::find(occupiedCells1.begin(), occupiedCells1.end(), x) != occupiedCells1.
				                       end();
		                       }).begin(),
		surroundCells1.end());
	occupiedCellsSize = occupiedCells1.size();
	surroundCellsSize = surroundCells1.size();

	data = new int[occupiedCellsSize + surroundCellsSize];
	auto occupiedCells = getOccupiedCells();
	auto surroundCells = getSurroundCells();
	std::ranges::copy(occupiedCells1, occupiedCells.begin());
	std::ranges::copy(surroundCells1, surroundCells.begin());
}

float Static::calculateAuraSize(const Urho3D::Vector3& boundingBox) const {
	auto gridSize = getGridSize();
	return Urho3D::Max(gridSize.x_, gridSize.y_) * 1.2f;
}

int Static::belowCloseLimit() const {
	const auto freeClose = Physical::belowCloseLimit();
	if (freeClose <= 0) {
		return 0;
	}
	return Urho3D::Min(freeClose, hasFreeSpace());
}

bool Static::hasAnyFreeSpace() const {
	return std::ranges::any_of(getSurroundCells(), [this](int index) { return canUse(index); });
}

int Static::hasFreeSpace() const {
	return std::ranges::count_if(getSurroundCells(), [this](int index) { return canUse(index); });
}

std::optional<std::tuple<Urho3D::Vector2, float>> Static::getPosToUseWithDist(Unit* user) {
	float minDistance = 9999999;
	Urho3D::Vector2 closest;
	int closestIndex = -1;

	for (auto index : getSurroundCells()) {
		if (canUse(index)) {
			const auto vec = Game::getEnvironment()->getCenter(index);
			setClosest(minDistance, closest, closestIndex, index, vec, user->getPosition());
		}
	}
	if (closestIndex >= 0) {
		return {{closest, minDistance}};
	}
	return {};
}

std::vector<int> Static::getIndexesForUse(Unit* user) const {
	std::vector<int> indexes;
	if (belowCloseLimit() <= 0) { return indexes; }
	indexes.reserve(surroundCellsSize);
	for (auto index : getSurroundCells()) {
		if (canUse(index)) {
			indexes.push_back(index);
		}
	}

	return indexes;
}

std::vector<int> Static::getIndexesForRangeUse(Unit* user) const {
	std::vector<int> indexes;
	if (belowRangeLimit() <= 0) { return indexes; }

	const std::vector<int> allIndexes = Game::getEnvironment()->getIndexesInRange(
		getPosition(), user->getLevel()->rangeAttackRange);
	auto cells = getOccupiedCells();

	for (auto index : allIndexes) {
		if (canUse(index)
			&& !isInCloseRange(index)
			&& std::ranges::find(cells, index) == cells.end()) {
			indexes.push_back(index);
		}

	}
	return indexes;
}

std::string Static::getValues(int precision) {
	const auto cordsCell = Game::getEnvironment()->getCords(indexInMainGrid);
	return Physical::getValues(precision)
		+ std::to_string(cordsCell.x_) + ","
		+ std::to_string(cordsCell.y_) + ","
		+ std::to_string(cast(state)) + ","
		+ std::to_string(cast(nextState)) + ",";
}
