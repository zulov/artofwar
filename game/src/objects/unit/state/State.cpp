#include "State.h"

#include <magic_enum.hpp>
#include "../Unit.h"
#include "utils/OtherUtils.h"

State::State(std::initializer_list<UnitState> active) {
	std::fill_n(nextStates, magic_enum::enum_count<UnitState>(), false);
	for (auto state : active) {
		nextStates[cast(state)] = true;
	}
}

bool State::canStart(Unit* unit, const ActionParameter& parameter) {
	return true;
}

bool State::validateTransition(UnitState stateTo) {
	return nextStates[cast(stateTo)];
}
