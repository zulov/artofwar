#pragma once
#include "Game.h"
#include "database/db_strcut.h"
#include "State.h"
#include "StateManager.h"
#include "StateUtils.h"
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
		//assert(parameter.index != parameter.thingToInteract->getMainBucketIndex());
		if (parameter.isFirstThingAlive()) {
			auto const indexesToUse = parameter.thingToInteract->getIndexesForUse(unit);
			return std::ranges::find(indexesToUse, unit->getMainBucketIndex()) != indexesToUse.end()
				&& parameter.thingToInteract->belowCloseLimit() > 0;
			//?&& parameter.thingToInteract->indexChanged()
			//&& !parameter.thingToInteract->isIndexSlotOccupied(parameter.index);
		}
		return false;
	}

	void setData(Unit* unit, int found, Physical* const thing) {
		setStartData(unit, found, thing);
		if (thing->getType() == ObjectType::UNIT) {
			setSlotData(unit, found, thing);
		}
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		auto const indexesToUse = parameter.thingToInteract->getIndexesForUse(unit);
		const auto found = std::ranges::find(indexesToUse, unit->getMainBucketIndex());
		assert(found != indexesToUse.end());
		unit->currentFrameState = 1;

		setData(unit, *found, parameter.thingToInteract);

		unit->maxSpeed = unit->dbLevel->maxSpeed / 2;
	}

	void onEnd(Unit* unit) override {
		if (unit->isFirstThingAlive()) {
			reduce(unit, unit->thingsToInteract[0]);
		}
		unit->maxSpeed = unit->dbLevel->maxSpeed;
		unit->thingsToInteract.clear();
		unit->indexToInteract = -1;
	}

	void reduce(Unit* unit, Physical* first) {
		first->reduceClose();
		first->setOccupiedIndexSlot(unit->slotToInteract, false);
	}

	void execute(Unit* unit, float timeStep) override {
		if (!unit->isFirstThingAlive()) {
			StateManager::toDefaultState(unit);
			return;
		}
		auto first = unit->thingsToInteract[0];
		if (unit->indexChanged() || first->indexChanged()) {
			reduce(unit, first);
			unit->thingsToInteract.clear();

			auto const indexesToUse = first->getIndexesForUse(unit);
			const auto found = std::ranges::find(indexesToUse, unit->getMainBucketIndex());
			if (found != indexesToUse.end()) {
				setData(unit, *found, first);
			} else {
				StateManager::toDefaultState(unit);
				return;
			}
		}
		if (unit->currentFrameState % unit->dbLevel->closeAttackReload == 0) {
			const auto val = first->getType() == ObjectType::UNIT
				                 ? unit->dbLevel->closeAttackVal
				                 : unit->dbLevel->buildingAttackVal;
			const auto [value, died] = first->absorbAttack(val);
			Game::getEnvironment()->addAttack(unit, value);
			Game::getPlayersMan()->getPlayer(unit->getPlayer())->addKilled(first);
		}
		++unit->currentFrameState;
	}
};
