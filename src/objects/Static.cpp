#include "Static.h"
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

	//const IntVector2 sizeX = calculateSize(gridSize.x_, bucketPosition.x_);
	//const IntVector2 sizeZ = calculateSize(gridSize.y_, bucketPosition.y_);

	// for (int i = sizeX.x_; i < sizeX.y_; ++i) {
	// 	for (int j = sizeZ.x_; j < sizeZ.y_; ++j) {
	//
	// 		}
	// }
	return {};
}

std::string Static::getValues(int precision) {
	return Physical::getValues(precision)
		+ std::to_string(bucketPosition.x_) + ","
		+ std::to_string(bucketPosition.y_) + ",";
}
