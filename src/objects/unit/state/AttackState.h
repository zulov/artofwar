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
		return unit->isFirstThingAlive() && !unit->thingsToInteract[0]->isSlotOccupied(unit->indexToInteract);
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->currentFrameState = 0;
		unit->thingsToInteract[0]->upClose();
		unit->thingsToInteract[0]->setOccupiedSlot(unit->indexToInteract, true);
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceClose();
			unit->thingsToInteract[0]->setOccupiedSlot(unit->indexToInteract, false);
		}
		unit->thingsToInteract.clear();
		unit->indexToInteract = -1;
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);
		if (unit->isFirstThingAlive()) {
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
