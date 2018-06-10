#include "Static.h"
#include "MathUtils.h"
#include "Game.h"
#include "simulation/env/Enviroment.h"
#include <string>

Static::Static(Vector3* _position, ObjectType _type) : Physical(_position, _type) {

}

Static::~Static() = default;

void Static::setBucketPosition(const IntVector2& _bucketPosition) {
	bucketPosition = _bucketPosition;
}

std::string Static::getColumns() {
	return Physical::getColumns() +
		+"bucket_x		INT     NOT NULL,"
		+ "bucket_y		INT     NOT NULL,";
}

Vector3 Static::getClosestCellPos(Vector3* pos) {
	const IntVector2 sizeX = calculateSize(gridSize.x_, bucketPosition.x_);
	const IntVector2 sizeZ = calculateSize(gridSize.y_, bucketPosition.y_);
	auto env = Game::getEnviroment();

	float closestDist = 999999;
	Vector2 closest;
	for (short i = sizeX.x_; i < sizeX.y_; ++i) {
		for (short j = sizeZ.x_; j < sizeZ.y_; ++j) {
			Vector2 vec = env->getCenter(i, j);
			float dist = sqDist(vec, *pos);
			if (dist < closestDist) {
				closestDist = dist;
				closest = closest;
			}
		}
	}
	return {};
}

std::string Static::getValues(int precision) {
	return Physical::getValues(precision)
		+ std::to_string(bucketPosition.x_) + ","
		+ std::to_string(bucketPosition.y_) + ",";
}
