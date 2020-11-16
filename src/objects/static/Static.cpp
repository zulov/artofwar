#include "Static.h"

#include <string>
#include "Game.h"
#include "math/MathUtils.h"
#include "../unit/Unit.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Environment.h"

Static::Static(Urho3D::Vector3& _position, int mainCell) : Physical(_position) {
	state = StaticState::ALIVE;
	nextState = StaticState::ALIVE;
	setMainCell(mainCell);
	isVisible = true;
}

void Static::load(dbload_static* dbloadStatic) {
	Physical::load(dbloadStatic);
	this->state = StaticState(dbloadStatic->state);
	this->nextState = StaticState(dbloadStatic->nextState);
}

void Static::setSurroundCells(const std::unordered_set<int>& indexes) {
	surroundCells.insert(surroundCells.begin(), indexes.begin(), indexes.end());
}

void Static::populate() {
	auto gridSize = getGridSize();
	const auto cordsCell = Game::getEnvironment()->getCords(mainCell);
	const auto sizeX = calculateSize(gridSize.x_, cordsCell.x_);
	const auto sizeZ = calculateSize(gridSize.y_, cordsCell.y_);

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

std::string Static::getColumns() {
	return Physical::getColumns() +
		+ "bucket_x		INT     NOT NULL,"
		+ "bucket_y		INT     NOT NULL,"
		+ "state		INT     NOT NULL,"
		+ "next_state		INT     NOT NULL,";
}

int Static::belowCloseLimit() {
	return Urho3D::Min(Physical::belowCloseLimit(), hasFreeSpace());
	//TODO PERF liczy sie hasFreeSpace nawet jak close limit 0
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

std::optional<std::tuple<Urho3D::Vector2, float, int>> Static::getPosToUseWithIndex(Unit* unit) {
	float minDistance = 999999;
	Urho3D::Vector2 closest;
	int closestIndex = -1;
	for (auto index : surroundCells) {
		if (canCollect(index)) {
			const auto vec = Game::getEnvironment()->getCenter(index);
			setClosest(minDistance, closest, closestIndex, index, vec, unit->getPosition());
		}
	}
	if (closestIndex >= 0) {
		return {{closest, minDistance, closestIndex}};
	}
	return {};
}

std::string Static::getValues(int precision) {
	const auto cordsCell = Game::getEnvironment()->getCords(mainCell);
	return Physical::getValues(precision)
		+ std::to_string(cordsCell.x_) + ","
		+ std::to_string(cordsCell.y_) + ","
		+ std::to_string(static_cast<char>(state)) + ","
		+ std::to_string(static_cast<char>(nextState)) + ",";
}
