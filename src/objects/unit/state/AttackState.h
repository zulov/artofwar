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

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		return parameter.isFirstThingAlive()
			&& !parameter.thingsToInteract[0]->isSlotOccupied(parameter.index);
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->currentFrameState = 0;
		
		unit->thingsToInteract.clear();
		unit->thingsToInteract.push_back(parameter.thingsToInteract[0]);
		unit->indexToInteract = parameter.index;

		unit->thingsToInteract[0]->upClose();
		unit->thingsToInteract[0]->setOccupiedSlot(unit->indexToInteract, true);
		unit->maxSpeed = unit->dbLevel->maxSpeed / 2;
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceClose();
			unit->thingsToInteract[0]->setOccupiedSlot(unit->indexToInteract, false);
			unit->thingsToInteract.clear();
		}
		unit->maxSpeed = unit->dbLevel->maxSpeed;
		unit->thingsToInteract.clear();
		unit->indexToInteract = -1;
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);
		if (unit->isFirstThingAlive() && unit->closeEnoughToAttack()) {
			if (fmod(unit->currentFrameState, 1 / unit->attackSpeed) < 1) {
				unit->thingsToInteract[0]->absorbAttack(unit->attackCoef);
			}
			++unit->currentFrameState;
		} else {
			StateManager::changeState(unit, UnitState::STOP);
		}
	}
};
