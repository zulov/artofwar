#include "GoState.h"
#include "../Unit.h"


GoState::GoState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
	transitions.insert(UnitStateType::GO);
	transitions.insert(UnitStateType::PATROL);
	transitions.insert(UnitStateType::FOLLOW);
	transitions.insert(UnitStateType::CHARAGE);
}


GoState::~GoState() = default;

void GoState::onStart(Unit* unit) {
}

void GoState::onStart(Unit* unit, ActionParameter& parameter) {
	unit->addAim(parameter.aim, parameter.aimAppend);
}


void GoState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void GoState::execute(Unit* unit) {
	State::execute(unit);
}
