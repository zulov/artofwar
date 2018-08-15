#pragma once
#include "State.h"

class DefendState : public State
{
public:
	DefendState() : State({UnitState::STOP, UnitState::DEAD}) {
	}

	~DefendState() = default;

	void onStart(Unit* unit, ActionParameter& parameter) override {
		unit->clearAims();
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
	}

	void execute(Unit* unit) override {
		State::execute(unit);
	}
};
