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
#include "env/Environment.h"


class AttackState : public State {
public:
	AttackState(): State({
		                     UnitState::STOP, UnitState::DEFEND, UnitState::DEAD,
		                     UnitState::GO, UnitState::FOLLOW, UnitState::CHARGE
	                     }) { }

	~AttackState() = default;

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		return parameter.isThingAlive()
			&& parameter.thingToInteract->indexCanBeUse(unit->getMainGridIndex())
			&& parameter.thingToInteract->belowCloseLimit() > 0;
		//sprawdzic cell limit
	}

	void setData(Unit* unit, Physical* const thing) {
		setStartData(unit, thing, CellState::ATTACK);
		if (thing->getType() == ObjectType::UNIT) {
			setSlotData(unit, dynamic_cast<Unit*>(thing));
		}
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		assert(parameter.thingToInteract->indexCanBeUse(unit->getMainGridIndex()));
		unit->currentFrameState = 0;

		setData(unit, parameter.thingToInteract);

		unit->maxSpeed = unit->dbLevel->maxSpeed / 2;
	}

	void onEnd(Unit* unit) override {
		reduce(unit);

		unit->maxSpeed = unit->dbLevel->maxSpeed;
	}

	void execute(Unit* unit, float timeStep) override {
		if (!unit->isFirstThingAlive()) {
			StateManager::toDefaultState(unit);
			return;
		}

		const auto first = unit->thingToInteract;
		if (unit->indexChanged() || first->indexChanged()) {
			reduce(unit);

			if (first->indexCanBeUse(unit->getMainGridIndex())) {
				setData(unit, first);
			} else {
				StateManager::toDefaultState(unit);
				return;
			}
		}
		++unit->currentFrameState;
		if (unit->currentFrameState >= unit->dbLevel->attackReload) {
			const auto val = unit->getAttackVal(unit);
			const auto [value, died] = first->absorbAttack(val);
			Game::getEnvironment()->addAttack(unit->getPlayer(), first->getPosition(), value);
			if (died) {
				Game::getPlayersMan()->getPlayer(unit->getPlayer())->addKilled(first);
			}
			unit->currentFrameState = 0;
		}
	}
};
