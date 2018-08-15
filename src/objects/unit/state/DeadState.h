#pragma once
#include "State.h"
#include "StateManager.h"

class DeadState : public State
{
public:
	DeadState() : State({UnitState::DISPOSE}) {
	}

	~DeadState() = default;

	void onStart(Unit* unit, ActionParameter& parameter) {
	}

	void onEnd(Unit* unit) {
		State::onEnd(unit);
	}

	void execute(Unit* unit) {
		State::execute(unit);
		StateManager::changeState(unit, UnitState::DISPOSE);
	}
};
