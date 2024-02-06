#pragma once
#include "Game.h"
#include "State.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "env/Environment.h"
#include "objects/CellState.h"
#include "objects/resource/ResourceEntity.h"

class CollectState : public State {
public:
	CollectState(): State({
		                      UnitState::STOP, UnitState::DEFEND, UnitState::DEAD,
		                      UnitState::GO, UnitState::FOLLOW, UnitState::CHARGE
	                      }) { }

	~CollectState() = default;

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		return parameter.isThingAlive()
			&& parameter.thingToInteract->indexCanBeUse(unit->getMainGridIndex());
		//sprawdzic cell limit
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		assert(parameter.thingToInteract->indexCanBeUse(unit->getMainGridIndex()));
		unit->currentFrameState = 0;

		setStartData(unit, unit->getMainGridIndex(), parameter.thingToInteract);

		unit->lastActionThingId = parameter.thingToInteract->getSecondaryId();
		unit->velocity = Urho3D::Vector2::ZERO;
		Game::getEnvironment()->updateCell(unit->getMainGridIndex(), 1, CellState::COLLECT);
	}

	void onEnd(Unit* unit) override {
		if (unit->isFirstThingAlive()) {
			unit->thingToInteract->reduceClose();
		}
		Game::getEnvironment()->updateCell(unit->indexToInteract, -1, CellState::NONE);

		unit->indexToInteract = -1;
	}

	void execute(Unit* unit, float timeStep) override {
		auto env = Game::getEnvironment();
		if (!unit->isFirstThingAlive()
			|| !env->cellInState(unit->getMainGridIndex(), CellState::COLLECT)) {
			StateManager::toDefaultState(unit);
			return;
		}
		const auto first = unit->thingToInteract;
		if (unit->indexChanged()) {
			first->reduceClose();
			unit->thingToInteract = nullptr;

			if (first->indexCanBeUse(unit->getMainGridIndex())) {
				setStartData(unit, unit->getMainGridIndex(), first);
			} else {
				StateManager::toDefaultState(unit);
				return;
			}
		}
		++unit->currentFrameState;
		if (unit->currentFrameState >= FRAMES_IN_PERIOD) {
			auto& resources = Game::getPlayersMan()->getPlayer(unit->player)->getResources();
			const auto resource = (ResourceEntity*)unit->thingToInteract;
			const auto bonus = resource->getBonus(unit->player);

			const auto [value, died] = resource->absorbAttack(unit->dbLevel->collect * bonus);

			env->addCollect(unit, resource->getId(), value);
			resources.add(resource->getId(), value);
			unit->currentFrameState = 0;
		}
	}
};
