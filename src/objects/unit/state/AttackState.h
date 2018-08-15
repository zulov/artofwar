#pragma once
#include "State.h"
#include "StateManager.h"
#include "objects/unit/Unit.h"

class AttackState : public State
{
public:
	AttackState(): State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD, UnitState::GO_TO, UnitState::FOLLOW, UnitState::CHARGE
	}) {
	}


	~AttackState() = default;

	bool canStart(Unit* unit) override {
		return unit->isFirstThingAlive();
	}

	void onStart(Unit* unit, ActionParameter& parameter) override {
		unit->velocity = Urho3D::Vector2::ZERO;
		unit->currentFrameState = 0;
		unit->thingsToInteract[0]->upClose();
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceClose();
		}
		unit->thingsToInteract.clear();
	}

	void execute(Unit* unit) override {
		State::execute(unit);
		if (unit->isFirstThingAlive()) {
			unit->velocity = Urho3D::Vector2::ZERO;
			if (fmod(unit->currentFrameState, 1 / unit->attackSpeed) < 1) {
				unit->thingsToInteract[0]->absorbAttack(unit->attackCoef);
			}
			++unit->currentFrameState;
		} else {
			unit->thingsToInteract.clear();
			StateManager::changeState(unit, UnitState::STOP);
		}
	}
};
