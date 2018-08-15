#pragma once
#include "../Unit.h"
#include "State.h"


class GoState : public State
{
public:
	GoState() : State({
		UnitState::STOP, UnitState::MOVE, UnitState::DEFEND, UnitState::DEAD, UnitState::GO_TO, UnitState::FOLLOW,
		UnitState::CHARGE
	}) {
	}


	~GoState() = default;

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
