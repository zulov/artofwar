#pragma once
#include "State.h"
#include "StateManager.h"

class DeadState : public State
{
public:
	DeadState() : State({UnitState::DISPOSE}) {
	}

	~DeadState() = default;

	void execute(Unit* unit, float timeStep) override {
		StateManager::changeState(unit, UnitState::DISPOSE);
	}
};
