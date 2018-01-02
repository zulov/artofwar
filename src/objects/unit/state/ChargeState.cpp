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
	unit->addAim(parameter);
	unit->maxSpeed = unit->dbUnit->maxSpeed * 2;
}

void ChargeState::onEnd(Unit* unit) {
	unit->maxSpeed = unit->dbUnit->maxSpeed;
}

void ChargeState::execute(Unit* unit) {
}
