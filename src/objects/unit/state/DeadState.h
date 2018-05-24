#pragma once
#include "State.h"
#include "StateManager.h"

class DeadState : public State
{
public:
	DeadState() {
		nextStates[static_cast<char>(UnitStateType::DISPOSE)] = true;
	}

	~DeadState() = default;

	void onStart(Unit* unit) {}

	void onStart(Unit* unit, ActionParameter& parameter) {}

	void onEnd(Unit* unit) {
		State::onEnd(unit);
	}

	void execute(Unit* unit) {
		State::execute(unit);
		StateManager::get()->changeState(unit, UnitStateType::DISPOSE);
	}
};
