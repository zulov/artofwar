#pragma once
#include "../Unit.h"
#include "State.h"


class StopState : public State
{
public:

	StopState() {
		nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
		nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
		nextStates[static_cast<char>(UnitStateType::GO)] = true;
		nextStates[static_cast<char>(UnitStateType::FOLLOW)] = true;
		nextStates[static_cast<char>(UnitStateType::ATTACK)] = true;
		nextStates[static_cast<char>(UnitStateType::MOVE)] = true;
		nextStates[static_cast<char>(UnitStateType::CHARAGE)] = true;
		nextStates[static_cast<char>(UnitStateType::COLLECT)] = true;
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
