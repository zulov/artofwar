#include "Static.h"

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
