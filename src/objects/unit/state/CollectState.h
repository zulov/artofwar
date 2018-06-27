#pragma once
#include "Game.h"
#include "State.h"
#include "objects/resource/ResourceEntity.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "simulation/env/Enviroment.h"


class CollectState : public State
{
public:

	CollectState() {
		nextStates[static_cast<char>(UnitState::STOP)] = true;
		nextStates[static_cast<char>(UnitState::DEFEND)] = true;
		nextStates[static_cast<char>(UnitState::DEAD)] = true;
		nextStates[static_cast<char>(UnitState::GO_TO)] = true;
		nextStates[static_cast<char>(UnitState::FOLLOW)] = true;
		nextStates[static_cast<char>(UnitState::CHARGE)] = true;
	}

	~CollectState() = default;

	void onStart(Unit* unit, ActionParameter& parameter) override {
		unit->velocity = Urho3D::Vector2::ZERO;
		unit->currentFrameState = 0;
		if (unit->isFirstThingAlive()
			&& Game::getEnviroment()->cellInState(unit->getBucketIndex(-1), CellState::RESOURCE)
			&& Game::getEnviroment()->belowCellLimit(unit->getBucketIndex(-1))) {
			unit->thingsToInteract[0]->upClose();

			Game::getEnviroment()->updateCell(unit->getBucketIndex(-1), 1);
		} else {
			unit->thingsToInteract.clear();
			StateManager::changeState(unit, UnitState::STOP);
		}
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceClose();
			Game::getEnviroment()->updateCell(unit->getBucketIndex(-1), -1);
		}
		unit->thingsToInteract.clear();
	}

	void execute(Unit* unit) override {
		State::execute(unit);
		if (unit->isFirstThingAlive()
			&& Game::getEnviroment()->cellInState(unit->getBucketIndex(-1), CellState::RESOURCE)) {
			auto& resources = Game::getPlayersManager()->getPlayer(unit->player)->getResources();
			auto resource = static_cast<ResourceEntity*>(unit->thingsToInteract[0]);
			const float value = resource->collect(unit->collectSpeed);
			resources.add(resource->getDbID(), value);
		} else {
			unit->thingsToInteract.clear();
			StateManager::changeState(unit, UnitState::STOP);
		}
	}
};
