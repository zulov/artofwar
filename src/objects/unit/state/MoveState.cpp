#include "MoveState.h"


MoveState::MoveState() {
	transitions.insert(UnitStateType::STOP);
	transitions.insert(UnitStateType::DEFEND);
	transitions.insert(UnitStateType::DEAD);
}


MoveState::~MoveState() {
}

void MoveState::onStart(Unit* unit) {
}

void MoveState::onEnd(Unit* unit) {
}

void MoveState::execute(Unit* unit) {
}
