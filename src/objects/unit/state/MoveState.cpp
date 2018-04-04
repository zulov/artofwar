#include "MoveState.h"


MoveState::MoveState() {
	transitions[static_cast<char>(UnitStateType::STOP)] = true;
	transitions[static_cast<char>(UnitStateType::DEFEND)] = true;
	transitions[static_cast<char>(UnitStateType::DEAD)] = true;
	transitions[static_cast<char>(UnitStateType::GO)] = true;
	transitions[static_cast<char>(UnitStateType::ATTACK)] = true;
	transitions[static_cast<char>(UnitStateType::PATROL)] = true;
	transitions[static_cast<char>(UnitStateType::FOLLOW)] = true;
	transitions[static_cast<char>(UnitStateType::CHARAGE)] = true;
	transitions[static_cast<char>(UnitStateType::COLLECT)] = true;
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
