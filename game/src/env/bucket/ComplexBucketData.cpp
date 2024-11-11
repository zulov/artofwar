#include "ComplexBucketData.h"
#include "objects/static/Static.h"
#include "CellEnums.h"
#include "utils/Flags.h"


void ComplexBucketData::setStatic(Static* object) {
	staticObj = object;
	if (object->getType() == ObjectType::BUILDING) {
		state = CellState::BUILDING;
		additionalInfo = object->getPlayer();
	} else {
		state = CellState::RESOURCE;
		additionalInfo = object->getId();
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
	auto& vals = resourceBonuses[player];

	auto& val = vals[resId];
	if (val < bonus) {
		val = bonus;
	}
}

void ComplexBucketData::resetResBonuses() {
	std::fill_n(&resourceBonuses[0][0], MAX_PLAYERS * RESOURCES_SIZE, 1.f);
}

float ComplexBucketData::getResBonus(char player, short resId) const {
	return resourceBonuses[player][resId];
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
			|| state == CellState::DEPLOY)//deplot bug?
		&& belowCellLimit();
}
