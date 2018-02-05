#include "FollowState.h"
#include "../Unit.h"


FollowState::FollowState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
	transitions.insert(UnitStateType::GO);
	transitions.insert(UnitStateType::PATROL);
}

FollowState::~FollowState() = default;

void FollowState::onStart(Unit* unit) {
}

void FollowState::onStart(Unit* unit, ActionParameter& parameter) {
	unit->removeAim();
	unit->addAim(parameter);
}


void FollowState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void FollowState::execute(Unit* unit) {
	State::execute(unit);
}
