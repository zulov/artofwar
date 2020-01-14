#include "ComplexBucketData.h"
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "objects/static/Static.h"
#include "consts.h"
#include <iostream>


ComplexBucketData::ComplexBucketData() {
	removeStatic();
}

ComplexBucketData::~ComplexBucketData() = default;

void ComplexBucketData::setStatic(Static* _object) {
	object = _object;
	size = 0;

	if (object->getType() == ObjectType::BUILDING) {
		state = CellState::BUILDING;
		additionalInfo = object->getPlayer();
	} else {
		state = CellState::RESOURCE;
		additionalInfo = object->getDbID();
	}
}

void ComplexBucketData::removeStatic() {
	object = nullptr;
	state = CellState::EMPTY;
	additionalInfo = -1;
	size = 0;
}

void ComplexBucketData::setCenter(float _centerX, float _centerY) {
	center = {_centerX, _centerY};
}

void ComplexBucketData::setEscapeThrought(int val) {
	escapeBucketIndex = val;
}

Urho3D::Vector2* ComplexBucketData::
getDirectrionFrom(Urho3D::Vector3& position, ComplexBucketData& escapeBucket) {
	return new Urho3D::Vector2(escapeBucket.getCenter().x_ - position.x_, escapeBucket.getCenter().y_ - position.z_);
}


void ComplexBucketData::setNeightOccupied(const unsigned char index) {
	isNeightOccupied |= Consts::bitFlags[index];
}

void ComplexBucketData::setNeightFree(const unsigned char index) {
	isNeightOccupied &= ~Consts::bitFlags[index];
}

bool ComplexBucketData::ifNeightIsFree(const unsigned char index) const {
	return !(isNeightOccupied & Consts::bitFlags[index]);
}

void ComplexBucketData::updateSize(char val, CellState cellState) {
	size += val;
	if (size <= 0) {
		state = CellState::EMPTY;
	} else if (state != CellState::NONE) {
		state = cellState;
	}
}

bool ComplexBucketData::belowCellLimit() const {
	return size < 2;
}

void ComplexBucketData::setDeploy(Building* building) {
	object = (Static*)building;
	state = CellState::DEPLOY;
}

void ComplexBucketData::removeDeploy() {
	removeStatic();
}

bool ComplexBucketData::isFreeToBuild(const short id) const {
	return isUnit();//TODO bug uwzglednic ID
}
