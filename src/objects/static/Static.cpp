#include "Static.h"

#include <string>
#include "Game.h"
#include "math/MathUtils.h"
#include "../unit/Unit.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Environment.h"
#include "utils/OtherUtils.h"

Static::Static(Urho3D::Vector3& _position, int mainCell, bool withNode) : Physical(_position, withNode),
                                                                          mainCell(mainCell),
                                                                          state(StaticState::ALIVE),
                                                                          nextState(StaticState::ALIVE) {
}

void Static::load(dbload_static* dbloadStatic) {
	Physical::load(dbloadStatic);
	this->state = StaticState(dbloadStatic->state);
	this->nextState = StaticState(dbloadStatic->nextState);
}

void Static::setSurroundCells(const std::vector<int>& indexes) {
	surroundCells = indexes;
}

void Static::populate() {
	auto gridSize = getGridSize();
	const auto cordsCell = Game::getEnvironment()->getCords(mainCell);
	const auto sizeX = calculateSize(gridSize.x_, cordsCell.x_);
	const auto sizeZ = calculateSize(gridSize.y_, cordsCell.y_);
	occupiedCells.reserve(gridSize.x_ * gridSize.y_);
	for (short i = sizeX.x_; i < sizeX.y_; ++i) {
		for (short j = sizeZ.x_; j < sizeZ.y_; ++j) {
			occupiedCells.push_back(Game::getEnvironment()->getIndex(i, j));
		}
	}
}

float Static::getAuraSize(const Urho3D::Vector3& boundingBox) const {
	auto gridSize = getGridSize();
	return Urho3D::Max(gridSize.x_, gridSize.y_) * 1.2f;
}

int Static::belowCloseLimit() {
	auto freeClose = Physical::belowCloseLimit();
	if (freeClose <= 0) {
		return 0;
	}
	return Urho3D::Min(freeClose, hasFreeSpace());
}

bool Static::hasAnyFreeSpace() const {
	for (auto index : surroundCells) {
		if (canCollect(index)) {
			return true;
		}
	}
	return false;
}

int Static::hasFreeSpace() const {
	int freeSpaces = 0;
	for (auto index : surroundCells) {
		if (canCollect(index)) {
			++freeSpaces;
		}
	}
	return freeSpaces;
}

bool Static::canCollect(int index) {
	return Game::getEnvironment()->cellIsCollectable(index);
}

std::optional<std::tuple<Urho3D::Vector2, float, int>> Static::getPosToUseWithIndex(Unit* user) {
	float minDistance = 999999;
	Urho3D::Vector2 closest;
	int closestIndex = -1;
	for (auto index : surroundCells) {
		if (canCollect(index)) {
			const auto vec = Game::getEnvironment()->getCenter(index);
			setClosest(minDistance, closest, closestIndex, index, vec, user->getPosition());
		}
	}
	if (closestIndex >= 0) {
		return {{closest, minDistance, closestIndex}};
	}
	return {};
}

std::vector<int> Static::getIndexesForUse(Unit* user) {
	std::vector<int> indexes;

	for (auto index : surroundCells) {
		if (canCollect(index)) {
			indexes.push_back(index);
		}
	}

	return indexes;
}


std::string Static::getValues(int precision) {
	const auto cordsCell = Game::getEnvironment()->getCords(mainCell);
	return Physical::getValues(precision)
		+ std::to_string(cordsCell.x_) + ","
		+ std::to_string(cordsCell.y_) + ","
		+ std::to_string(cast(state)) + ","
		+ std::to_string(cast(nextState)) + ",";
}
