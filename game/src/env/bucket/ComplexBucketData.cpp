#include "ComplexBucketData.h"
#include "objects/static/Static.h"
#include "objects/CellState.h"
#include "utils/Flags.h"


ComplexBucketData::ComplexBucketData() {
	clear();
}

void ComplexBucketData::setStatic(Static* object) {
	size = 0;

	if (object->getType() == ObjectType::BUILDING) {
		state = CellState::BUILDING;
		additionalInfo = object->getPlayer();
	} else {
		state = CellState::RESOURCE;
		additionalInfo = object->getId();
	}
	for (auto& resourceBonus : resourceBonuses) {
		std::fill_n(resourceBonus, RESOURCES_SIZE, 1.f);
	}
}

void ComplexBucketData::clear() {
	state = CellState::NONE;
	additionalInfo = -1;
	size = 0;
}

Urho3D::Vector2 ComplexBucketData::getDirectionFrom(const Urho3D::Vector3& position, Urho3D::Vector2 centerEscape) {
	return {centerEscape.x_ - position.x_, centerEscape.y_ - position.z_};
}

void ComplexBucketData::setNeightOccupied(const unsigned char index) {
	isNeightOccupied |= Flags::bitFlags[index];
}

void ComplexBucketData::setNeightFree(const unsigned char index) {
	isNeightOccupied &= ~Flags::bitFlags[index];
}

float ComplexBucketData::getCost() const { return cost; }

bool ComplexBucketData::isBuildable() const {
	return state == CellState::NONE
		|| state == CellState::COLLECT;
}

void ComplexBucketData::updateSize(char val, CellState cellState) {//TODO przemyslec to 
	size += val;
	if (size <= 0) {
		size = 0;
		if (state == CellState::ATTACK || state == CellState::COLLECT) {
			state = CellState::NONE;
		}
	} else {
		if (state == CellState::NONE || state == CellState::ATTACK || state == CellState::COLLECT) {
			state = cellState;
		}
	}
}

void ComplexBucketData::setResBonuses(char player, const std::vector<char>& resIds, float bonus) {
	auto& vals = resourceBonuses[player];
	for (const char resId : resIds) {
		auto& val = vals[resId];
		if (val < bonus) {
			val = bonus;
		}
	}
}

void ComplexBucketData::resetResBonuses(char player, char resId) {
	resourceBonuses[player][resId] = 1.f;
}

float ComplexBucketData::getResBonus(char player, short resId) const {
	return resourceBonuses[player][resId];
}

bool ComplexBucketData::belowCellLimit() const {
	return size < 2;
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
			|| state == CellState::DEPLOY)
		&& belowCellLimit();
}