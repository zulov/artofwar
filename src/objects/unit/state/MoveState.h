#pragma once
#include "State.h"

class MoveState : public State
{
public:

	MoveState(): State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD, UnitState::GO_TO, UnitState::ATTACK, UnitState::COLLECT,
		UnitState::SHOT, UnitState::FOLLOW, UnitState::CHARGE
	}) {
	}


	~MoveState() = default;

	void onStart(Unit* unit, const ActionParameter& parameter) {
		unit->removeCurrentAim();
	}

	void onEnd(Unit* unit) {
		State::onEnd(unit);
	}

	void execute(Unit* unit, float timeStep) {
		State::execute(unit, timeStep);
	}
};
