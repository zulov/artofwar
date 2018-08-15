#include "State.h"
#include "../Unit.h"

State::State() {
	std::fill_n(nextStates, STATE_SIZE, false);
}

State::~State() = default;

bool State::canStart(Unit* unit) {
	return true;
}

void State::onEnd(Unit* unit) {
	unit->atState = false;
}

void State::execute(Unit* unit) {
	unit->atState = true;
}

bool State::validateTransition(UnitState stateTo) {
	return nextStates[static_cast<char>(stateTo)];
}
