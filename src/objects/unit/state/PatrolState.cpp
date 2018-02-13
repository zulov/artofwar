#include "PatrolState.h"
#include "../Unit.h"


PatrolState::PatrolState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
	transitions.insert(UnitStateType::GO);
}


PatrolState::~PatrolState() = default;

void PatrolState::onStart(Unit* unit) {
}

void PatrolState::onStart(Unit* unit, ActionParameter& parameter) {
	unit->addAim(parameter.aim);
}


void PatrolState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void PatrolState::execute(Unit* unit) {
	State::execute(unit);
}
