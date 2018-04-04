#include "PatrolState.h"
#include "../Unit.h"


PatrolState::PatrolState() {
	transitions[static_cast<char>(UnitStateType::STOP)] = true;
	transitions[static_cast<char>(UnitStateType::DEFEND)] = true;
	transitions[static_cast<char>(UnitStateType::DEAD)] = true;
	transitions[static_cast<char>(UnitStateType::GO)] = true;
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
