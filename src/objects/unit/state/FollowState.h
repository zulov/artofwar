#pragma once
#include "../Unit.h"
#include "State.h"


class FollowState : public State
{
public:
	FollowState() : State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD,
		UnitState::GO_TO, UnitState::MOVE, UnitState::FOLLOW,
		UnitState::COLLECT, UnitState::ATTACK
	}) {
	}

	~FollowState() = default;

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->setAim(parameter.aim);
		unit->thingsToInteract.clear();
	}

	void execute(Unit* unit, float timeStep)override {
		if (!unit->aims.hasCurrent()) {
			StateManager::toDefaultState(unit);
		}
	}

	void onEnd(Unit* unit) override {
		unit->removeCurrentAim();
	}

};
