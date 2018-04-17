#include "FollowState.h"
#include "../Unit.h"


FollowState::FollowState() {
	transitions[static_cast<char>(UnitStateType::STOP)] = true;
	transitions[static_cast<char>(UnitStateType::DEFEND)] = true;
	transitions[static_cast<char>(UnitStateType::DEAD)] = true;
	transitions[static_cast<char>(UnitStateType::GO)] = true;
	transitions[static_cast<char>(UnitStateType::PATROL)] = true;
}

FollowState::~FollowState() = default;

void FollowState::onStart(Unit* unit) {
}

void FollowState::onStart(Unit* unit, ActionParameter& parameter) {
	unit->clearAims();
	unit->addAim(parameter.aim, parameter.aimAppend);
}


void FollowState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void FollowState::execute(Unit* unit) {
	State::execute(unit);
}
