#include "Formation.h"
#include "objects/unit/Unit.h"
#include <algorithm>
#include <iostream>


Formation::Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2 _direction) : id(_id),
	type(_type), direction(_direction), state(FormationState::FORMING) {

	for (auto value : *_units) {
		units.push_back(dynamic_cast<Unit*>(value));
	}
	type = _type;
	updateIds();
	update();
}

Formation::~Formation() = default;

void Formation::updateIds() {
	short k = 0;
	for (auto unit : units) {
		unit->setFormation(id);
		unit->setPositionInFormation(k++);
	}
	leaderId = units.size() / 2;
}

void Formation::updateSizes() {
	sideA = sqrt(units.size());
	sideB = units.size() / sideA;

	sizeA = (sideA - 1) * sparsity;
	sizeB = (sideB - 1) * sparsity;
}

void Formation::update() {
	switch (state) {
	case FormationState::FORMING:
		if (wellFormed < theresholed) {
			changeState(FormationState::MOVING);

		}
	case FormationState::MOVING:
		updateUnits();

		if (!units.empty()) {
			updateIds();
			updateCenter();
			updateSizes();
		} else {
			changeState(FormationState::EMPTY);
		}
		break;
	case FormationState::REACHED:
		for (auto unit : units) {
			unit->setFormation(-1);
			unit->setPositionInFormation(-1);
		}
		units.clear();
		break;
	}
}

void Formation::changeState(FormationState newState) {
	state = newState;
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

void Formation::updateUnits() {
	units.erase(
	            std::remove_if(
	                           units.begin(), units.end(),
	                           [this](Unit* unit)
	                           {
		                           return !unit->isAlive() || unit->getFormation() != id;
	                           }),
	            units.end());
}

void Formation::updateCenter() {
	Vector3* leaderPos = units[leaderId]->getPosition();
	center = Vector2(
	                 leaderPos->x_,
	                 leaderPos->z_
	                );
}
