#include "State.h"
#include "../Unit.h"

State::State(std::initializer_list<UnitState> active) {
	std::fill_n(nextStates, STATE_SIZE, false);
	for (auto state : active) {
		nextStates[static_cast<char>(state)] = true;
	}
}

State::~State() = default;

bool State::canStart(Unit* unit) {
	return true;
}

void State::onEnd(Unit* unit) {
	unit->atState = false;
}

void State::execute(Unit* unit, float timeStep) {
	unit->atState = true;
}

bool State::validateTransition(UnitState stateTo) {
	return nextStates[static_cast<char>(stateTo)];
}
