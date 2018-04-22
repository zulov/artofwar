#include "Formation.h"
#include "Game.h"
#include "commands/CommandList.h"
#include "commands/action/ActionCommand.h"
#include "commands/action/FormationAction.h"
#include "objects/unit/Unit.h"
#include "objects/unit/aim/FutureAims.h"
#include <algorithm>
#include <iostream>


Formation::Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2& _direction) : id(_id),
	type(_type), direction(_direction), state(FormationState::FORMING) {

	for (auto value : *_units) {
		auto unit = dynamic_cast<Unit*>(value);
		unit->clearAims();
		units.push_back(unit);
	}

	if (!units.empty()) {
		updateIds();
		updateCenter();
	} else {
		changeState(FormationState::EMPTY);
	}
}

Formation::~Formation() = default;

void Formation::electLeader() {
	Vector2 localCenter = Vector2::ZERO;
	for (auto unit : units) {
		auto pos = unit->getPosition();
		localCenter.x_ += pos->x_;
		localCenter.y_ += pos->z_;
	}
	localCenter /= units.size();
	int maxDist = 9999999;
	leader = nullptr;
	for (int i = 0; i < units.size(); ++i) {
		auto pos = units[i]->getPosition();
		auto currentPos = Vector2(pos->x_, pos->z_);

		auto sqDist = (currentPos - localCenter).LengthSquared();
		if (sqDist < maxDist) {
			leader = units[i];
			maxDist = sqDist;
		}
	}
	if (oldLeader != nullptr
		&& leader != oldLeader
		&& !futureOrders.empty()) {

		if (oldLeader->getFormation() == id) {
			oldLeader->clearAims();
		}
		changeState(FormationState::FORMING);
	}
	oldLeader = leader;
}

void Formation::updateIds() {
	if (changed) {
		short k = 0;
		for (auto unit : units) {
			unit->setFormation(id);
			unit->setPositionInFormation(k++);
		}
		electLeader();
		updateSizes();
		changed = false;
	}
}

void Formation::updateSizes() {
	sideA = sqrt(units.size()) + 0.5;
	sideB = units.size() / sideA;

	sizeA = (sideA - 1) * sparsity;
	sizeB = (sideB - 1) * sparsity;
}

void Formation::calculateNotWellFormed() {
	notWellFormed = 0;
	notWellFormedExact = 0;
	for (auto unit : units) {
		auto pos = unit->getPosition();

		auto currentPos = Vector2(pos->x_, pos->z_);
		auto desiredPos = getPositionFor(unit->getPositionInFormation());
		auto sqDist = (currentPos - desiredPos).LengthSquared();
		if (sqDist > 2 * 2) {
			notWellFormed += 1;
		}
		if (sqDist > 1) {
			notWellFormedExact += 1;
		}
	}
	notWellFormed /= units.size();
	notWellFormedExact /= units.size();
}

void Formation::innerUpdate() {
	updateUnits();

	if (!units.empty()) {
		updateIds();
		updateCenter();
		calculateNotWellFormed();
	} else {
		changeState(FormationState::EMPTY);
	}
}

void Formation::update() {
	switch (state) {
	case FormationState::FORMING:
		innerUpdate();
		if (notWellFormed < theresholedMin) {
			changeState(FormationState::MOVING);
			if (!futureOrders.empty()) {
				for (const auto& futureOrder : futureOrders) {
					Game::get()->getActionCommandList()->add(new FormationAction(this, futureOrder.action,
					                                                             new Vector2(futureOrder.vector)));
				}
			}
		}
		break;
	case FormationState::MOVING:
		innerUpdate();
		if (notWellFormed > theresholedMax) {
			changeState(FormationState::FORMING);
		} else if (notWellFormedExact == 0
			&& !leader->hasAim()) {
			changeState(FormationState::REACHED);
			futureOrders.clear();
		}
		break;
	case FormationState::REACHED:
		for (auto unit : units) {
			unit->resetFormation();
		}
		units.clear();
		break;
	}
}

void Formation::changeState(FormationState newState) {
	state = newState;
}

Vector2 Formation::getPositionFor(short id) {
	const int columnThis = id % sideA;
	const int rowThis = id / sideA;

	short leaderID = leader->getPositionInFormation();

	const int columnLeader = leaderID % sideA;
	const int rowLeader = leaderID / sideA;

	const int column = columnThis - columnLeader;
	const int row = rowThis - rowLeader;

	return center - Vector2(column * sparsity, row * sparsity);
}

float Formation::getPriority(int id) const {
	return 1;
}

std::optional<Physical*> Formation::getLeader() {
	if (state != FormationState::REACHED && units.size() > leader->getPositionInFormation()) {
		return leader;
	}
	return std::nullopt;
}

void Formation::appendFutureTarget(const Vector2& _futureTarget, const Physical* _physical, OrderType _action) {
	futureOrders.emplace_back(_futureTarget, _physical, _action);
}

void Formation::addFutureTarget(const Vector2& _futureTarget, const Physical* _physical, OrderType _action) {
	futureOrders.clear();
	futureOrders.emplace_back(_futureTarget, _physical, _action);
}

size_t Formation::getSize() {
	return units.size();
}

void Formation::semiReset() {
	notWellFormed = 1;
	notWellFormedExact = 1;
	changed = true;
	futureOrders.clear();
	changeState(FormationState::FORMING);
}

void Formation::updateUnits() {
	units.erase(
	            std::remove_if(
	                           units.begin(), units.end(),
	                           [this](Unit* unit)
	                           {
		                           bool ifErase = !unit->isAlive() || unit->getFormation() != id;
		                           if (ifErase) {
			                           if (unit->getFormation() == id) {
				                           unit->resetFormation();
			                           }
			                           changed = true;
		                           }
		                           return ifErase;
	                           }),
	            units.end());
}

void Formation::updateCenter() {
	Vector3* leaderPos = leader->getPosition();
	center = Vector2(leaderPos->x_, leaderPos->z_);
}
