#include "Static.h"
#include "Game.h"
#include "MathUtils.h"
#include "simulation/env/Enviroment.h"
#include <string>

Static::Static(Vector3* _position, ObjectType _type) : Physical(_position, _type) {
}

Static::~Static() = default;

void Static::setMainCell(const IntVector2& _mainCell) {
	mainCell = _mainCell;
}

void Static::populate(const IntVector2& size) {
	gridSize = size;
	const IntVector2 sizeX = calculateSize(gridSize.x_, mainCell.x_);
	const IntVector2 sizeZ = calculateSize(gridSize.y_, mainCell.y_);

	auto env = Game::getEnviroment();

	for (short i = sizeX.x_; i < sizeX.y_; ++i) {
		for (short j = sizeZ.x_; j < sizeZ.y_; ++j) {
			ocupiedCells.push_back(env->getIndex(i, j));
		}
	}
}

std::string Static::getColumns() {
	return Physical::getColumns() +
		+"bucket_x		INT     NOT NULL,"
		+ "bucket_y		INT     NOT NULL,";
}

Vector2 Static::getClosestCellPos(Vector3* pos) const {
	const IntVector2 sizeX = calculateSize(gridSize.x_, mainCell.x_);
	const IntVector2 sizeZ = calculateSize(gridSize.y_, mainCell.y_);
	auto env = Game::getEnviroment();

	float closestDist = 999999;//TODO do zastapienia tym z unita
	Vector2 closest;
	for (auto index : ocupiedCells) {
		Vector2 vec = env->getCenter(index);
		float dist = sqDist(vec, *pos);
		if (dist < closestDist) {
			closestDist = dist;
			closest = vec;
		}
	}
	return closest;
}

Vector2 Static::getPosToFollow(Vector3* center) const {
	return getClosestCellPos(center);
}

std::string Static::getValues(int precision) {
	return Physical::getValues(precision)
		+ std::to_string(mainCell.x_) + ","
		+ std::to_string(mainCell.y_) + ",";
}
