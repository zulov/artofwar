#pragma once
#include "../Unit.h"
#include "State.h"


class StopState : public State
{
public:

	StopState() {
		nextStates[static_cast<char>(UnitState::DEFEND)] = true;
		nextStates[static_cast<char>(UnitState::DEAD)] = true;
		nextStates[static_cast<char>(UnitState::GO)] = true;
		nextStates[static_cast<char>(UnitState::FOLLOW)] = true;
		nextStates[static_cast<char>(UnitState::ATTACK)] = true;
		nextStates[static_cast<char>(UnitState::MOVE)] = true;
		nextStates[static_cast<char>(UnitState::CHARGE)] = true;
		nextStates[static_cast<char>(UnitState::COLLECT)] = true;
	}

	~StopState() = default;

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
