#include "ComplexBucketData.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "objects/Static.h"
#include <iostream>


ComplexBucketData::ComplexBucketData() {
	removeStatic();
	neighbours.reserve(8);
	size = 0;
}

ComplexBucketData::~ComplexBucketData() = default;

void ComplexBucketData::setStatic(Static* _object) {
	object = _object;
	if (object->getType() == ObjectType::BUILDING) {
		type = CellState::BUILDING;
	} else {
		type = CellState::RESOURCE;
	}

	if (_object->getType() == ObjectType::BUILDING) {
		additonalInfo = _object->getPlayer();
	} else {
		additonalInfo = _object->getDbID();
	}
	size = 0;
}

void ComplexBucketData::removeStatic() {
	object = nullptr;
	type = CellState::EMPTY;
	additonalInfo = -1;
}

void ComplexBucketData::setCenter(float _centerX, float _centerY) {
	center = Vector2(_centerX, _centerY);
}

void ComplexBucketData::setEscapeThrought(int val) {
	escapeBucketIndex = val;
}

Urho3D::Vector2* ComplexBucketData::
getDirectrionFrom(Urho3D::Vector3* position, ComplexBucketData& escapeBucket) {
	return new Vector2(escapeBucket.getCenter().x_ - position->x_, escapeBucket.getCenter().y_ - position->z_);
}
