#pragma once
#include "../Unit.h"
#include "State.h"


class FollowState : public State
{
public:
	FollowState() : State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD,
		UnitState::GO_TO, UnitState::MOVE, UnitState::FOLLOW
	}) {
	}

	~FollowState() = default;

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->setAim(parameter.aim);
	}


	void onEnd(Unit* unit) override {
		State::onEnd(unit);
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);
	}
};
