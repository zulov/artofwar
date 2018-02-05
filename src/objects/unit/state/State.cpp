#include "State.h"
#include "../Unit.h"

State::State() = default;

State::~State() = default;

void State::onEnd(Unit* unit) {
	unit->atState = false;
}

void State::execute(Unit* unit) {
	unit->atState = true;
}

bool State::validateTransition(UnitStateType stateTo) {
	return transitions.find(stateTo) != transitions.end();
}
