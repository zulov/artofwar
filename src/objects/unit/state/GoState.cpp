#include "GoState.h"
#include "../Unit.h"


GoState::GoState() {
	nextStates[static_cast<char>(UnitStateType::STOP)] = true;
	nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
	nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
	nextStates[static_cast<char>(UnitStateType::GO)] = true;
	nextStates[static_cast<char>(UnitStateType::PATROL)] = true;
	nextStates[static_cast<char>(UnitStateType::FOLLOW)] = true;
	nextStates[static_cast<char>(UnitStateType::CHARAGE)] = true;
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
