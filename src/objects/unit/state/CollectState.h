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
			&& unit->getMainCell() == unit->indexToInteract //TODO je¿eli jest inny to sprobowaæ podmienic
			&& Game::getEnviroment()->cellInState(unit->getMainCell(), {CellState::RESOURCE, CellState::EMPTY})
			&& Game::getEnviroment()->belowCellLimit(unit->getMainCell())) {
			unit->thingsToInteract[0]->upClose();

			Game::getEnviroment()->updateCell(unit->getMainCell(), 1, CellState::RESOURCE);
		} else {
			StateManager::changeState(unit, UnitState::STOP);
		}
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceClose();
			Game::getEnviroment()->updateCell(unit->getMainCell(), -1, CellState::NONE);
		}
		unit->indexToInteract = -1;
	}

	void execute(Unit* unit) override {
		State::execute(unit);
		if (unit->isFirstThingAlive()
			&& unit->getMainCell() == unit->indexToInteract
			&& Game::getEnviroment()->cellInState(unit->getMainCell(), {CellState::RESOURCE})) {
			//TODO musi byc dokladnie w dobry mbuckecie
			auto& resources = Game::getPlayersManager()->getPlayer(unit->player)->getResources();
			auto resource = static_cast<ResourceEntity*>(unit->thingsToInteract[0]);
			const float value = resource->collect(unit->collectSpeed);
			resources.add(resource->getDbID(), value);
		} else {
			StateManager::changeState(unit, UnitState::STOP);
		}
	}
};
