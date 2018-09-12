#include "Static.h"
#include "Game.h"
#include "MathUtils.h"
#include "simulation/env/Enviroment.h"
#include <string>

Static::Static(Urho3D::Vector3* _position, ObjectType _type, int mainCell) : Physical(_position, _type) {
	state = StaticState::ALIVE;
	nextState = StaticState::ALIVE;
	setMainCell(mainCell);
}

Static::~Static() = default;

void Static::setMainCell(int _mainCell) {
	mainCell = _mainCell;
}

void Static::setNextState(StaticState stateTo) {
	nextState = stateTo;
}

void Static::setState(StaticState state) {
	this->state = state;
}

void Static::load(dbload_static* dbloadStatic) {
	Physical::load(dbloadStatic);
	this->state = StaticState(dbloadStatic->state);
	this->nextState = StaticState(dbloadStatic->nextState);
}

void Static::populate(const Urho3D::IntVector2& size) {
	gridSize = size;
	const auto cordsCell = Game::getEnviroment()->getCords(mainCell);
	const auto sizeX = calculateSize(gridSize.x_, cordsCell.x_);
	const auto sizeZ = calculateSize(gridSize.y_, cordsCell.y_);

	for (short i = sizeX.x_; i < sizeX.y_; ++i) {
		for (short j = sizeZ.x_; j < sizeZ.y_; ++j) {
			occupiedCells.push_back(Game::getEnviroment()->getIndex(i, j));
		}
	}
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
	const auto type = Game::getEnviroment()->getType(index);
	return (type == CellState::EMPTY || type == CellState::COLLECT) //TODO collect or attack
		&& Game::getEnviroment()->getCurrentSize(index) <= 2;
}

Urho3D::Vector2 Static::getPosToFollow(Urho3D::Vector3* center) const {
	auto [vec, index] = getPosToFollowWithIndex(center);
	return vec;
}

std::tuple<Urho3D::Vector2, int> Static::getPosToFollowWithIndex(Urho3D::Vector3* center) const {
	float closestDist = 999999;
	Urho3D::Vector2 closest;
	int closestIndex = -1;
	for (auto index : surroundCells) {
		if (canCollect(index)) {
			const auto vec = Game::getEnviroment()->getCenter(index);
			const float dist = sqDist(vec, *center);
			if (dist < closestDist) {
				closestDist = dist;
				closest = vec;
				closestIndex = index;
			}
		}
	}
	return {closest, closestIndex};
}

std::string Static::getValues(int precision) {
	const auto cordsCell = Game::getEnviroment()->getCords(mainCell);
	return Physical::getValues(precision)
		+ std::to_string(cordsCell.x_) + ","
		+ std::to_string(cordsCell.y_) + ","
		+ std::to_string(static_cast<char>(state)) + ","
		+ std::to_string(static_cast<char>(nextState)) + ",";
}
