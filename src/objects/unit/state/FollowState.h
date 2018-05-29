#pragma once
#include "State.h"
#include "../Unit.h"



class FollowState : public State
{
public:
	FollowState() {
		nextStates[static_cast<char>(UnitState::STOP)] = true;
		nextStates[static_cast<char>(UnitState::DEFEND)] = true;
		nextStates[static_cast<char>(UnitState::DEAD)] = true;
		nextStates[static_cast<char>(UnitState::GO_TO)] = true;
		nextStates[static_cast<char>(UnitState::MOVE)] = true;
	}

	~FollowState() = default;

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
