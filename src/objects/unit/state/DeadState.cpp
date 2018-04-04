#include "DeadState.h"
#include "../Unit.h"


DeadState::DeadState() {
	transitions[static_cast<char>(UnitStateType::DISPOSE)] = true;
}


DeadState::~DeadState() = default;

void DeadState::onStart(Unit* unit) {

}

void DeadState::onStart(Unit* unit, ActionParameter& parameter) {
}

void DeadState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void DeadState::execute(Unit* unit) {
	State::execute(unit);
	StateManager::get()->changeState(unit, UnitStateType::DISPOSE);
}
