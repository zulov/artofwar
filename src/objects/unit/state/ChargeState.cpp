#include "ChargeState.h"
#include "../Unit.h"


ChargeState::ChargeState() {
	nextStates[static_cast<char>(UnitStateType::STOP)] = true;
	nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
	nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
	nextStates[static_cast<char>(UnitStateType::GO)] = true;
	nextStates[static_cast<char>(UnitStateType::PATROL)] = true;
	nextStates[static_cast<char>(UnitStateType::FOLLOW)] = true;
	nextStates[static_cast<char>(UnitStateType::CHARAGE)] = true;
}


ChargeState::~ChargeState() = default;

void ChargeState::onStart(Unit* unit) {
}

void ChargeState::onStart(Unit* unit, ActionParameter& parameter) {
	unit->addAim(parameter.aim);
	//TODO aim?
	unit->maxSpeed = unit->dbLevel->maxSpeed * 2;
}

void ChargeState::onEnd(Unit* unit) {
	State::onEnd(unit);
	unit->maxSpeed = unit->dbLevel->maxSpeed;
}

void ChargeState::execute(Unit* unit) {
	State::execute(unit);
}
