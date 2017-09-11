#include "AttackState.h"


AttackState::AttackState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
}


AttackState::~AttackState() {
}

void AttackState::onStart(Unit* unit) {
}

void AttackState::onStart(Unit* unit, ActionParameter* parameter) {
}

void AttackState::onEnd(Unit* unit) {
}

void AttackState::execute(Unit* unit) {
}
