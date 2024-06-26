#pragma once
#include "../Unit.h"
#include "State.h"


class GoState : public State {
public:
	GoState() : State({
		UnitState::STOP, UnitState::MOVE, UnitState::DEFEND,
		UnitState::DEAD, UnitState::GO, UnitState::FOLLOW,
		UnitState::CHARGE
	}) { }

	~GoState() = default;

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		return parameter.aim != nullptr;
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->setAim(parameter.aim);
		unit->thingToInteract = nullptr;
	}

	void onEnd(Unit* unit) override {
		unit->removeCurrentAim();
	}

	void execute(Unit* unit, float timeStep) override {
		if (!unit->aims.hasCurrent() && !unit->hasStateChangePending()) {
			StateManager::toDefaultState(unit);
		}
	}
};
