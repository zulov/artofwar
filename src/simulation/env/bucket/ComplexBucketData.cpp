#include "ComplexBucketData.h"
#include "objects/static/Static.h"
#include "objects/CellState.h"
#include "utils/consts.h"


ComplexBucketData::ComplexBucketData() {
	removeStatic();
}

void ComplexBucketData::setStatic(Static* _object) {
	object = _object;
	size = 0;

	if (object->getType() == ObjectType::BUILDING) {
		state = CellState::BUILDING;
		additionalInfo = object->getPlayer();
	} else {
		state = CellState::RESOURCE;
		additionalInfo = object->getId();
	}
}

void ComplexBucketData::removeStatic() {
	object = nullptr;
	state = CellState::NONE;
	additionalInfo = -1;
	size = 0;
}

void ComplexBucketData::setEscapeThrough(int val) {
	escapeBucketIndex = val;
}

Urho3D::Vector2 ComplexBucketData::getDirectionFrom(Urho3D::Vector3& position, Urho3D::Vector2 centerEscape) {
	return {centerEscape.x_ - position.x_, centerEscape.y_ - position.z_};
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

bool ComplexBucketData::isPassable() const {
	return state == CellState::NONE
		|| state == CellState::ATTACK
		|| state == CellState::COLLECT
		|| state == CellState::DEPLOY;
}

void ComplexBucketData::updateSize(char val, CellState cellState) {
	size += val;
	if (size <= 0) {
		state = CellState::NONE;
	} else {
		state = cellState;
	} //to jakies takie nie 
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
