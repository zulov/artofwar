#include "DisposeState.h"
#include "../Unit.h"


DisposeState::DisposeState()= default;


DisposeState::~DisposeState() = default;

void DisposeState::onStart(Unit* unit) {
}

void DisposeState::onStart(Unit* unit, ActionParameter& parameter) {
}

void DisposeState::onEnd(Unit* unit) {
	State::onEnd(unit);
}

void DisposeState::execute(Unit* unit) {
	State::execute(unit);
}
