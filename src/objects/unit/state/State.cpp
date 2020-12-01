#include "State.h"

#include <magic_enum.hpp>

#include "../Unit.h"

State::State(std::initializer_list<UnitState> active) {
	std::fill_n(nextStates, magic_enum::enum_count<UnitState>(), false);
	for (auto state : active) {
		nextStates[static_cast<char>(state)] = true;
	}
}

bool State::canStart(Unit* unit, const ActionParameter& parameter) {
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
