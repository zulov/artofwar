#pragma once
#include "State.h"

class DefendState : public State
{
public:
	DefendState() {
		nextStates[static_cast<char>(UnitStateType::STOP)] = true;
		nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
	}

	~DefendState() = default;

	void onStart(Unit* unit) override {
		unit->clearAims();
	}

	void onStart(Unit* unit, ActionParameter& parameter) override {}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
	}

	void execute(Unit* unit) override {
		State::execute(unit);
	}
};
