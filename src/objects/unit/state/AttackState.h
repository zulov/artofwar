#pragma once
#include "Game.h"
#include "database/db_strcut.h"
#include "State.h"
#include "StateManager.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/Unit.h"
#include "UnitState.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"


class AttackState : public State {
public:
	AttackState(): State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD,
		UnitState::GO, UnitState::FOLLOW, UnitState::CHARGE
	}) { }

	~AttackState() = default;

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		return parameter.isFirstThingAlive()
			&& unit->getMainBucketIndex() == parameter.index
			&& parameter.thingToInteract->isFirstThingInSameSocket()
			&& !parameter.thingToInteract->isSlotOccupied(parameter.index);
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->currentFrameState = 0;

		unit->thingsToInteract.clear();
		unit->thingsToInteract.push_back(parameter.thingToInteract);
		unit->indexToInteract = parameter.index;

		unit->thingsToInteract[0]->upClose();
		unit->thingsToInteract[0]->setOccupiedSlot(unit->indexToInteract, true);
		unit->maxSpeed = unit->dbLevel->maxSpeed / 2;
	}

	void onEnd(Unit* unit) override {
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceClose();
			unit->thingsToInteract[0]->setOccupiedSlot(unit->indexToInteract, false);
			unit->thingsToInteract.clear();
		}
		unit->maxSpeed = unit->dbLevel->maxSpeed;
		unit->thingsToInteract.clear();
		unit->indexToInteract = -1;
	}

	bool isInRange(Unit* unit) {
		return unit->isFirstThingAlive()
			&& unit->isInRightSocket()
			&& unit->thingsToInteract[0]->isFirstThingInSameSocket();
	}

	void execute(Unit* unit, float timeStep) override {
		if (isInRange(unit)) {
			auto a =fmod(unit->currentFrameState, 1.f / unit->dbLevel->closeAttackSpeed);
			if (fmod(unit->currentFrameState, 1.f / unit->dbLevel->closeAttackSpeed) < 1) {
				const auto [value, died] = unit->thingsToInteract[0]->absorbAttack(unit->dbLevel->closeAttackVal);
				Game::getEnvironment()->addAttack(unit, value);
				Game::getPlayersMan()->getPlayer(unit->getPlayer())->addKilled(unit->thingsToInteract[0]);
			}
			++unit->currentFrameState;
		} else {
			StateManager::toDefaultState(unit);
		}
	}
};
