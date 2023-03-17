#pragma once
#include "../Unit.h"
#include "State.h"


class FollowState : public State {
public:
	FollowState() : State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD,
		UnitState::GO, UnitState::MOVE, UnitState::FOLLOW,
		UnitState::COLLECT, UnitState::ATTACK
	}) { }

	~FollowState() = default;

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		return parameter.aim != nullptr;
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->setAim(parameter.aim);
		unit->thingToInteract = nullptr;
	}

	void execute(Unit* unit, float timeStep) override {
		if (!unit->aims.hasCurrent() && !unit->hasStateChangePending()) {
			StateManager::toDefaultState(unit);
		}
	}

	void onEnd(Unit* unit) override {
		unit->removeCurrentAim();
	}

};
