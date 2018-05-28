#pragma once
#include "../Unit.h"
#include "State.h"


class GoState : public State
{
public:
	GoState() {
		nextStates[static_cast<char>(UnitState::STOP)] = true;
		nextStates[static_cast<char>(UnitState::DEFEND)] = true;
		nextStates[static_cast<char>(UnitState::DEAD)] = true;
		nextStates[static_cast<char>(UnitState::GO)] = true;
		nextStates[static_cast<char>(UnitState::FOLLOW)] = true;
		nextStates[static_cast<char>(UnitState::CHARAGE)] = true;
	}


	~GoState() = default;

	void onStart(Unit* unit) override {}

	void onStart(Unit* unit, ActionParameter& parameter) override {
		unit->addAim(parameter.aim);
		//TODO aim?
	}


	void onEnd(Unit* unit) override {
		State::onEnd(unit);
	}

	void execute(Unit* unit) override {
		State::execute(unit);
	}
};
