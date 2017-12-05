#include "Static.h"
#include <string>

Static::Static(Vector3* _position, ObjectType _type) : Physical(_position, _type) {

}

Static::~Static() {
}

IntVector2& Static::getBucketPosition() {
	return bucketPosition;
}

void Static::setBucketPosition(IntVector2 _bucketPosition) {
	bucketPosition = _bucketPosition;
}

IntVector2& Static::getGridSize() {
	return gridSize;
}

std::string Static::getColumns() {
	return Physical::getColumns() +
		+"bucket_x		INT     NOT NULL,"
		+ "bucket_y		INT     NOT NULL,";
}

std::string Static::getValues(int precision) {
	return Physical::getValues(precision)
		+ std::to_string(bucketPosition.x_) + ","
		+ std::to_string(bucketPosition.y_) + ",";
}
