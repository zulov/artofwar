#include "ChargeState.h"
#include "../Unit.h"


ChargeState::ChargeState() {
	transitions[static_cast<char>(UnitStateType::STOP)] = true;
	transitions[static_cast<char>(UnitStateType::DEFEND)] = true;
	transitions[static_cast<char>(UnitStateType::DEAD)] = true;
	transitions[static_cast<char>(UnitStateType::GO)] = true;
	transitions[static_cast<char>(UnitStateType::PATROL)] = true;
	transitions[static_cast<char>(UnitStateType::FOLLOW)] = true;
	transitions[static_cast<char>(UnitStateType::CHARAGE)] = true;
}


ChargeState::~ChargeState() = default;

void ChargeState::onStart(Unit* unit) {
}

void ChargeState::onStart(Unit* unit, ActionParameter& parameter) {
	unit->addAim(parameter.aim, parameter.aimAppend);
	unit->maxSpeed = unit->dbLevel->maxSpeed * 2;
}

void ChargeState::onEnd(Unit* unit) {
	State::onEnd(unit);
	unit->maxSpeed = unit->dbLevel->maxSpeed;
}

void ChargeState::execute(Unit* unit) {
	State::execute(unit);
}
