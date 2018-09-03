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

void Static::populate(const Urho3D::IntVector2& size) {
	gridSize = size;
	const auto cordsCell = Game::getEnviroment()->getCords(mainCell);
	const auto sizeX = calculateSize(gridSize.x_, cordsCell.x_);
	const auto sizeZ = calculateSize(gridSize.y_, cordsCell.y_);

	for (short i = sizeX.x_; i < sizeX.y_; ++i) {
		for (short j = sizeZ.x_; j < sizeZ.y_; ++j) {
			ocupiedCells.push_back(Game::getEnviroment()->getIndex(i, j));
		}
	}
}

std::string Static::getColumns() {
	return Physical::getColumns() +
		+ "bucket_x		INT     NOT NULL,"
		+ "bucket_y		INT     NOT NULL,";
}

unsigned char Static::belowCloseLimit() {
	return Urho3D::Min(Physical::belowCloseLimit(), hasFreeSpace());
	//TODO PERF liczy sie hasFreeSpace nawet jak close limit 0
}

unsigned char Static::hasFreeSpace() const {
	auto env = Game::getEnviroment();
	unsigned char freeSpaces = 0;
	for (auto index : surroundCells) {
		if (canCollect(index, env->getType(index))) {
			++freeSpaces;
		}
	}
	return freeSpaces;
}

bool Static::canCollect(int index, CellState type) const {
	return (type == CellState::EMPTY || type == CellState::COLLECT) //TODO collect or attack
		&& Game::getEnviroment()->getCurrentSize(index) <= 2;
}

Urho3D::Vector2 Static::getPosToFollow(Urho3D::Vector3* center) const {
	float closestDist = 999999;
	Urho3D::Vector2 closest;
	for (auto index : surroundCells) {
		auto type = Game::getEnviroment()->getType(index);
		if (canCollect(index, type)) {
			const auto vec = Game::getEnviroment()->getCenter(index);
			const float dist = sqDist(vec, *center);
			if (dist < closestDist) {
				closestDist = dist;
				closest = vec;
			}
		}
	}
	return closest;
}

std::tuple<Urho3D::Vector2, int> Static::getPosToFollowWithIndex(Urho3D::Vector3* center) const {
	float closestDist = 999999;
	Urho3D::Vector2 closest;
	int closestIndex = -1;
	for (auto index : surroundCells) {
		auto type = Game::getEnviroment()->getType(index);
		if (canCollect(index, type)) {
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
		+ std::to_string(cordsCell.y_) + ",";
}
