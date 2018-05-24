#pragma once
#include "State.h"

class PatrolState : public State
{
public:
	PatrolState() {
		nextStates[static_cast<char>(UnitStateType::STOP)] = true;
		nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
		nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
		nextStates[static_cast<char>(UnitStateType::GO)] = true;
	}

	~PatrolState() = default;

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
