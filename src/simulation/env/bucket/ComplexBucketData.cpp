#include "ComplexBucketData.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "objects/static/Static.h"
#include <iostream>


ComplexBucketData::ComplexBucketData() {
	removeStatic();
	neighbours.reserve(8);
}

ComplexBucketData::~ComplexBucketData() = default;

void ComplexBucketData::setStatic(Static* _object) {
	object = _object;
	size = 0;

	if (object->getType() == ObjectType::BUILDING) {
		type = CellState::BUILDING;
		additonalInfo = object->getPlayer();
	} else {
		type = CellState::RESOURCE;
		additonalInfo = object->getDbID();
	}
}

void ComplexBucketData::removeStatic() {
	object = nullptr;
	type = CellState::EMPTY;
	additonalInfo = -1;
	size = 0;
}

void ComplexBucketData::setCenter(float _centerX, float _centerY) {
	center = {_centerX, _centerY};
}

void ComplexBucketData::setEscapeThrought(int val) {
	escapeBucketIndex = val;
}

Urho3D::Vector2* ComplexBucketData::
getDirectrionFrom(Urho3D::Vector3* position, ComplexBucketData& escapeBucket) {
	return new Urho3D::Vector2(escapeBucket.getCenter().x_ - position->x_, escapeBucket.getCenter().y_ - position->z_);
}

void ComplexBucketData::updateSize(char val, CellState cellState) {
	size += val;
	if (size <= 0) {
		type = CellState::EMPTY;
	} else if (type != CellState::NONE) {
		type = cellState;
	}
}

bool ComplexBucketData::belowCellLimit() {
	return size < 2;
}
