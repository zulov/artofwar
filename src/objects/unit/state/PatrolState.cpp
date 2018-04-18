#include "PatrolState.h"
#include "../Unit.h"


PatrolState::PatrolState() {
	nextStates[static_cast<char>(UnitStateType::STOP)] = true;
	nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
	nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
	nextStates[static_cast<char>(UnitStateType::GO)] = true;
}


PatrolState::~PatrolState() = default;

void PatrolState::onStart(Unit* unit) {
}

void PatrolState::onStart(Unit* unit, ActionParameter& parameter) {
	unit->addAim(parameter.aim, parameter.aimAppend);
}


void PatrolState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void PatrolState::execute(Unit* unit) {
	State::execute(unit);
}
