#include "Formation.h"
#include "objects/unit/Unit.h"
#include <algorithm>
#include <iostream>
#include "Game.h"
#include "commands/action/ActionCommandList.h"
#include "commands/action/ActionCommand.h"


Formation::Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2 _direction) : id(_id),
	type(_type), direction(_direction), state(FormationState::FORMING) {

	for (auto value : *_units) {
		units.push_back(dynamic_cast<Unit*>(value));
	}
	type = _type;
	updateIds();
	updateUnits();
	calculateNotWellFormed();

	if (!units.empty()) {
		updateIds();
		updateCenter();
		updateSizes();
	} else {
		changeState(FormationState::EMPTY);
	}
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
	const short sideB = units.size() / sideA;

	sizeA = (sideA - 1) * sparsity;
	sizeB = (sideB - 1) * sparsity;
}

void Formation::calculateNotWellFormed() {
	notWellFormed = 0;
	for (auto unit : units) {
		auto pos = unit->getPosition();

		auto currentPos = Vector2(pos->x_, pos->z_);
		auto desiredPos = getPositionFor(unit->getPositionInFormation());
		auto sqDist = (currentPos - desiredPos).LengthSquared();
		if (sqDist > 3 * 3) {
			notWellFormed += 1;
		}
	}
	notWellFormed /= units.size();
}

void Formation::update() {
	switch (state) {
	case FormationState::FORMING:
		if (notWellFormed < theresholed) {
			changeState(FormationState::MOVING);
			Game::get()->getActionCommandList()->add(new ActionCommand(this, action, new Vector2(futureTarget)));
		}
	case FormationState::MOVING:
		updateUnits();

		if (!units.empty()) {
			updateIds();
			updateCenter();
			updateSizes();
			calculateNotWellFormed();
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

Physical* Formation::getLeader() {
	return units[leaderId];
}

void Formation::setFutureTarget(const Vector2& _futureTarget, OrderType _action) {
	futureTarget = _futureTarget;
	action = _action;
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
	center = Vector2(leaderPos->x_, leaderPos->z_);
}
