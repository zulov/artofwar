#include "Static.h"
#include "Game.h"
#include "MathUtils.h"
#include "simulation/env/Enviroment.h"
#include <string>

Static::Static(Urho3D::Vector3* _position, ObjectType _type) : Physical(_position, _type) {
	state = StaticState::ALIVE;
	nextState = StaticState::ALIVE;
}

Static::~Static() = default;

void Static::setMainCell(const Urho3D::IntVector2& _mainCell) {
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
	const auto sizeX = calculateSize(gridSize.x_, mainCell.x_);
	const auto sizeZ = calculateSize(gridSize.y_, mainCell.y_);

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

bool Static::belowCloseLimit() {
	return Physical::belowCloseLimit() && hasFreeSpace();

}

bool Static::hasFreeSpace() const {
	auto env = Game::getEnviroment();
	for (auto index : surroundCells) {
		if (canCollect(index, env->getType(index))) {
			return true;
		}
	}
	return false;
}

bool Static::canCollect(int index, CellState type) const {
	return (type == CellState::EMPTY || type == CellState::COLLECT) //TODO collect or attack
		&& Game::getEnviroment()->getCurrentSize(index) <= 2;
}

bool Static::isToDispose() const {
	return state == StaticState::DISPOSE;
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

std::string Static::getValues(int precision) {
	return Physical::getValues(precision)
		+ std::to_string(mainCell.x_) + ","
		+ std::to_string(mainCell.y_) + ",";
}
