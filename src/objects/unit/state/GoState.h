#pragma once
#include "../Unit.h"
#include "State.h"


class GoState : public State
{
public:
	GoState() : State({
		UnitState::STOP, UnitState::MOVE, UnitState::DEFEND, 
		UnitState::DEAD, UnitState::GO_TO, UnitState::FOLLOW,
		UnitState::CHARGE
	}) {
	}


	~GoState() = default;

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->setAim(parameter.aim);
		unit->thingsToInteract.clear();
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);
	}
};
