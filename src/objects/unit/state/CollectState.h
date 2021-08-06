#pragma once
#include "Game.h"
#include "State.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "simulation/env/Environment.h"
#include "objects/CellState.h"

class CollectState : public State {
public:
	CollectState(): State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD,
		UnitState::GO, UnitState::FOLLOW, UnitState::CHARGE
	}) { }

	~CollectState() = default;

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		//assert(parameter.index != parameter.thingToInteract->getMainBucketIndex());
		if (parameter.isFirstThingAlive()) {
			auto const indexesToUse = parameter.thingToInteract->getIndexesForUse(unit);
			return std::ranges::find(indexesToUse, unit->getMainBucketIndex()) != indexesToUse.end();
			//&& Game::getEnvironment()->cellIsCollectable(unit->getMainBucketIndex())
			//?&& parameter.thingToInteract->indexChanged()
			//&& !parameter.thingToInteract->isIndexSlotOccupied(parameter.index);
		}
		return false;
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {

		auto const indexesToUse = parameter.thingToInteract->getIndexesForUse(unit);
		const auto found = std::ranges::find(indexesToUse, unit->getMainBucketIndex());
		assert(found != indexesToUse.end());
		unit->currentFrameState = 1;
		setStartData(unit, *found, parameter.thingToInteract);

		unit->lastActionThingId = parameter.thingToInteract->getId();
		unit->velocity = Urho3D::Vector2::ZERO;
		Game::getEnvironment()->updateCell(unit->getMainBucketIndex(), 1, CellState::COLLECT);
	}

	void onEnd(Unit* unit) override {
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceClose();
		}
		Game::getEnvironment()->updateCell(unit->indexToInteract, -1, CellState::NONE);
		//assert(unit->getMainBucketIndex() == unit->indexToInteract);
		unit->indexToInteract = -1;
	}

	void execute(Unit* unit, float timeStep) override {
		if (!unit->isFirstThingAlive() || !Game::getEnvironment()->cellInState(
			unit->getMainBucketIndex(), CellState::COLLECT)) {
			StateManager::toDefaultState(unit);
			return;
		}
		auto first = unit->thingsToInteract[0];
		if (unit->indexChanged()) {
			first->reduceClose();
			unit->thingsToInteract.clear();

			auto const indexesToUse = first->getIndexesForUse(unit);
			const auto found = std::ranges::find(indexesToUse, unit->getMainBucketIndex());
			if (found != indexesToUse.end()) {
				setStartData(unit, *found, first);
			} else {
				StateManager::toDefaultState(unit);
				return;
			}
		}

		auto& resources = Game::getPlayersMan()->getPlayer(unit->player)->getResources();
		auto resource = unit->thingsToInteract[0];
		const auto [value, died] = resource->absorbAttack(unit->dbLevel->collectSpeed * timeStep);
		Game::getEnvironment()->addCollect(unit, value);
		resources.add(resource->getId(), value);
	}
};
