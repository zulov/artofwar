#include "State.h"

State::State() {

}

State::~State() {

}

bool State::validateTransition(UnitStateType stateTo) {
	return transitions.find(stateTo) != transitions.end();
}
