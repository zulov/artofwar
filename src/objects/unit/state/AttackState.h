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
		if (parameter.isFirstThingAlive()) {
			auto const indexesToUse = parameter.thingToInteract->getIndexesForUse(unit);
			return std::ranges::find(indexesToUse, unit->getMainGridIndex()) != indexesToUse.end()
				&& parameter.thingToInteract->belowCloseLimit() > 0;
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
		const auto found = std::ranges::find(indexesToUse, unit->getMainGridIndex());
		assert(found != indexesToUse.end());
		unit->currentFrameState = 0;

		setData(unit, *found, parameter.thingToInteract);

		unit->maxSpeed = unit->dbLevel->maxSpeed / 2;
		Game::getEnvironment()->updateCell(unit->getMainGridIndex(), 1, CellState::ATTACK);
	}

	void onEnd(Unit* unit) override {
		reduce(unit, unit->thingToInteract);

		unit->maxSpeed = unit->dbLevel->maxSpeed;
	}

	void reduce(Unit* unit, Physical* first) {
		if (unit->isFirstThingAlive()) {
			first->reduceClose();
			first->setOccupiedIndexSlot(unit->slotToInteract, false);
		}
		if (unit->indexToInteract > 0) {
			Game::getEnvironment()->updateCell(unit->indexToInteract, -1, CellState::NONE);
		}

		unit->thingToInteract = nullptr;
		unit->indexToInteract = -1;
	}

	void execute(Unit* unit, float timeStep) override {
		if (!unit->isFirstThingAlive()) {
			StateManager::toDefaultState(unit);
			return;
		}
		const auto first = unit->thingToInteract;
		if (unit->indexChanged() || first->indexChanged()) {
			reduce(unit, first);

			auto const indexesToUse = first->getIndexesForUse(unit);
			const auto found = std::ranges::find(indexesToUse, unit->getMainGridIndex());
			if (found != indexesToUse.end()) {
				setData(unit, *found, first);
			} else {
				StateManager::toDefaultState(unit);
				return;
			}
		}
		if (unit->currentFrameState >= unit->dbLevel->attackReload) {
			const auto val = unit->getAttackVal(unit);
			const auto [value, died] = first->absorbAttack(val);
			Game::getEnvironment()->addAttack(unit->getPlayer(), first->getPosition(), value);
			if (died) {
				Game::getPlayersMan()->getPlayer(unit->getPlayer())->addKilled(first);
			}
			unit->currentFrameState = 0;
		}
		++unit->currentFrameState;
	}
};
