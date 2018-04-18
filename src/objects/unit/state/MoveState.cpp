#include "MoveState.h"


MoveState::MoveState() {
	nextStates[static_cast<char>(UnitStateType::STOP)] = true;
	nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
	nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
	nextStates[static_cast<char>(UnitStateType::GO)] = true;
	nextStates[static_cast<char>(UnitStateType::ATTACK)] = true;
	nextStates[static_cast<char>(UnitStateType::PATROL)] = true;
	nextStates[static_cast<char>(UnitStateType::FOLLOW)] = true;
	nextStates[static_cast<char>(UnitStateType::CHARAGE)] = true;
	nextStates[static_cast<char>(UnitStateType::COLLECT)] = true;
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
