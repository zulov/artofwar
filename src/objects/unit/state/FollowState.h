#pragma once
#include "State.h"
#include "../Unit.h"



class FollowState : public State
{
public:
	FollowState() {
		nextStates[static_cast<char>(UnitStateType::STOP)] = true;
		nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
		nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
		nextStates[static_cast<char>(UnitStateType::GO)] = true;
		nextStates[static_cast<char>(UnitStateType::PATROL)] = true;
		nextStates[static_cast<char>(UnitStateType::MOVE)] = true;
	}

	~FollowState() = default;

	void onStart(Unit* unit) override {}

	void onStart(Unit* unit, ActionParameter& parameter) override {
		unit->clearAims();
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
