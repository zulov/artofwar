#pragma once
#include "State.h"
#include "../Unit.h"


class FollowState : public State
{
public:
	FollowState() : State({UnitState::STOP, UnitState::DEFEND, UnitState::DEAD, UnitState::GO_TO, UnitState::MOVE}) {
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
