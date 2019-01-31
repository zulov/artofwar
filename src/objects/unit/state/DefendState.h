#pragma once
#include "State.h"

class DefendState : public State
{
public:
	DefendState() : State({UnitState::STOP, UnitState::DEAD}) {
	}

	~DefendState() = default;

	void onStart(Unit* unit, const ActionParameter& parameter) override {

	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);
	}
};
