#include "Formation.h"
#include "objects/unit/Unit.h"
#include <algorithm>
#include <iostream>


void Formation::updateIds() {
	short k = 0;
	for (auto unit : units) {
		unit->setFormation(id);
		unit->setPositionInFormation(k++);
	}
	leaderId = units.size() / 2;
}

Formation::Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2 _direction) : id(_id),
	type(_type), direction(_direction), state(FormationState::FORMING) {

	for (auto value : *_units) {
		units.push_back(dynamic_cast<Unit*>(value));
	}
	type = _type;
	sideA = sqrt(units.size());
	sideB = units.size() / sideA;

	sizeA = (sideA - 1) * sparsity;
	sizeB = (sideB - 1) * sparsity;

	updateIds();
	updateCenter();
}


Formation::~Formation() = default;

bool Formation::update() {
	updateUnits();
	if (!units.empty()) {
		updateCenter();
		return true;
	}
	return false;
}

Vector2 Formation::getPositionFor(short id) const {
	int column = id % sideA;
	int row = id / sideA;
	if (id != leaderId) {
		return center - Vector2(column * sparsity - sizeA / 2, row * sparsity - sizeB / 2);
	}
	return center;
}

float Formation::getPriority(int id) const {
	return 1;
}

float Formation::isReady() {
	return 0;
}

void Formation::updateUnits() {
	units.erase(
	            std::remove_if(
	                           units.begin(), units.end(),
	                           [this](Unit* unit)
	                           {
		                           return !unit->isAlive() || unit->getFormation() != id;
	                           }),
	            units.end());
	updateIds();
}

void Formation::updateCenter() {

	Vector3* leaderPos = units[leaderId]->getPosition();
	center = Vector2(
	                 leaderPos->x_,
	                 leaderPos->z_
	                );
}
