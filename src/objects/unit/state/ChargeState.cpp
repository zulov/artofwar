#include "ChargeState.h"
#include "../Unit.h"


ChargeState::ChargeState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
	transitions.insert(UnitStateType::GO);
	transitions.insert(UnitStateType::PATROL);
	transitions.insert(UnitStateType::FOLLOW);
	transitions.insert(UnitStateType::CHARAGE);
}


ChargeState::~ChargeState() = default;

void ChargeState::onStart(Unit* unit) {
}

void ChargeState::onStart(Unit* unit, ActionParameter& parameter) {
	unit->addAim(parameter.aim);
	unit->maxSpeed = unit->dbUnit->maxSpeed * 2;
}

void ChargeState::onEnd(Unit* unit) {
	State::onEnd(unit);
	unit->maxSpeed = unit->dbUnit->maxSpeed;
}

void ChargeState::execute(Unit* unit) {
	State::execute(unit);
}
