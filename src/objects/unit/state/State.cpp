#include "State.h"

State::State() = default;

State::~State() = default;

bool State::validateTransition(UnitStateType stateTo) {
	return transitions.find(stateTo) != transitions.end();
}
