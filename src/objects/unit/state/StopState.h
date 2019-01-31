#pragma once
#include "../Unit.h"
#include "State.h"


class StopState : public State
{
public:

	StopState(): State({
		UnitState::DEFEND, UnitState::DEAD, UnitState::GO_TO, UnitState::FOLLOW, UnitState::ATTACK,
		UnitState::COLLECT, UnitState::SHOT, UnitState::MOVE, UnitState::CHARGE, UnitState::COLLECT
	}) {
	}

	~StopState() = default;

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->thingsToInteract.clear();
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);
	}
};
