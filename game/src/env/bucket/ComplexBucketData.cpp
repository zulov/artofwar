#include "ComplexBucketData.h"

#include <algorithm>
#include "objects/static/Static.h"
#include "CellEnums.h"
#include "utils/Flags.h"


void ComplexBucketData::setStatic(Static* object) {
	staticObj = object;
	if (object->getType() == ObjectType::BUILDING) {
		state = CellState::BUILDING;
		additionalInfo = object->getPlayer();
	}
	else {
		state = CellState::RESOURCE;
		additionalInfo = object->getDbId();
	}
}

void ComplexBucketData::clear() {
	state = CellState::NONE;
	additionalInfo = -1;
	staticObj = nullptr;
}

void ComplexBucketData::setNeightOccupied(const unsigned char index) {
	isNeightOccupied |= Flags::bitFlags[index];
}

void ComplexBucketData::setNeightFree(const unsigned char index) {
	isNeightOccupied &= ~Flags::bitFlags[index];
}

int ComplexBucketData::getCost() const { return cost; }

bool ComplexBucketData::isBuildable() const {
	return state == CellState::NONE
		|| state == CellState::COLLECT;
}

void ComplexBucketData::incStateSize(CellState cellState) {
	++size;
	if (state == CellState::NONE || state == CellState::ATTACK || state == CellState::COLLECT) {
		state = cellState;
	}
}
void ComplexBucketData::decStateSize() {
	--size;
	if (size <= 0) {
		assert(size == 0);
		if (state == CellState::ATTACK || state == CellState::COLLECT) {
			state = CellState::NONE;
		}
	}
}

void ComplexBucketData::setResBonuses(char player, unsigned char resId, float bonus) {
	if (resourceBonuses == nullptr) {//TODO mem perf dodaæ jakis pool obiektow
		resourceBonuses = new float[MAX_PLAYERS * RESOURCES_SIZE];
		resourceBonuses[player * MAX_PLAYERS + resId] = bonus; return;
	}
	auto& val = resourceBonuses[player * MAX_PLAYERS + resId];

	val = std::max(val, bonus);
}

void ComplexBucketData::resetResBonuses() {
	delete resourceBonuses;
	resourceBonuses = nullptr;
}

float ComplexBucketData::getResBonus(char player, short resId) const {
	return resourceBonuses == nullptr ? 1.f : resourceBonuses[player * MAX_PLAYERS + resId];
}

bool ComplexBucketData::belowCellLimit() const {
	return size < 3;
}

void ComplexBucketData::setDeploy() {
	state = CellState::DEPLOY;
}

bool ComplexBucketData::cellIsCollectable() const {
	return (state == CellState::NONE
		|| state == CellState::COLLECT)
		&& belowCellLimit();
}

bool ComplexBucketData::cellIsAttackable() const {
	return (state == CellState::NONE
		|| state == CellState::ATTACK
		|| state == CellState::DEPLOY)//deploy bug?
		&& belowCellLimit();
}
