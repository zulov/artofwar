#include "MoveState.h"


MoveState::MoveState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
	transitions.insert(UnitStateType::GO);
	transitions.insert(UnitStateType::ATTACK);
	transitions.insert(UnitStateType::PATROL);
	transitions.insert(UnitStateType::FOLLOW);
	transitions.insert(UnitStateType::CHARAGE);
	transitions.insert(UnitStateType::COLLECT);
}


MoveState::~MoveState() = default;

void MoveState::onStart(Unit* unit) {
}

void MoveState::onStart(Unit* unit, ActionParameter& parameter) {
}


void MoveState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void MoveState::execute(Unit* unit) {
	State::execute(unit);
}
