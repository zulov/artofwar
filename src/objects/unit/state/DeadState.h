#pragma once
#include "State.h"
#include "StateManager.h"

class DeadState : public State
{
public:
	DeadState() : State({UnitState::DISPOSE}) {
	}

	~DeadState() = default;

	void onStart(Unit* unit, const ActionParameter& parameter) override {
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);
		StateManager::changeState(unit, UnitState::DISPOSE);
	}
};
