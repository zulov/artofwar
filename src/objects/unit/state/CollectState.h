#pragma once
#include "Game.h"
#include "State.h"
#include "objects/resource/ResourceEntity.h"
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
	}) {
	}

	~CollectState() = default;

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		return parameter.isFirstThingAlive()
			&& unit->getMainBucketIndex() == parameter.index //TODO je¿eli jest inny to sprobowaæ podmienic
			&& Game::getEnvironment()->cellIsCollectable(unit->getMainBucketIndex())
			&& parameter.thingToInteract->belowCloseLimit() > 0;
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->velocity = Urho3D::Vector2::ZERO;
		unit->currentFrameState = 0;

		unit->thingsToInteract.clear();
		unit->thingsToInteract.push_back(parameter.thingToInteract);
		unit->indexToInteract = parameter.index;

		unit->thingsToInteract[0]->upClose();
		unit->lastActionThingId = parameter.thingToInteract->getId();

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
		if (unit->isFirstThingAlive()
			&& unit->getMainBucketIndex() == unit->indexToInteract
			&& Game::getEnvironment()->cellInState(unit->getMainBucketIndex(), CellState::COLLECT)) {
			//TODO musi byc dokladnie w dobry mbuckecie
			auto& resources = Game::getPlayersMan()->getPlayer(unit->player)->getResources();
			auto resource = dynamic_cast<ResourceEntity*>(unit->thingsToInteract[0]);
			const float value = resource->collect(unit->dbLevel->collectSpeed * timeStep);
			Game::getEnvironment()->addCollect(unit, value);
			resources.add(resource->getId(), value);
		} else {
			StateManager::toDefaultState(unit);
		}
	}
};
