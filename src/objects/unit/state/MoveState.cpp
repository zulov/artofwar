#include "MoveState.h"


MoveState::MoveState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
	transitions.insert(UnitStateType::GO);
	transitions.insert(UnitStateType::PATROL);
	transitions.insert(UnitStateType::FOLLOW);
}


MoveState::~MoveState() {
}

void MoveState::onStart(Unit* unit) {
}
void MoveState::onStart(Unit* unit, ActionParameter* parameter) {
}


void MoveState::onEnd(Unit* unit) {
}

void MoveState::execute(Unit* unit) {
}
