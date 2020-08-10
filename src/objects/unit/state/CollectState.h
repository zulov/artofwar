#pragma once
#include "Game.h"
#include "State.h"
#include "objects/resource/ResourceEntity.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "simulation/env/Environment.h"


class CollectState : public State
{
public:

	CollectState(): State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD,
		UnitState::GO_TO, UnitState::FOLLOW, UnitState::CHARGE
	}) {
	}

	~CollectState() = default;

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		return parameter.isFirstThingAlive()
			&& unit->getMainCell() == parameter.index //TODO je¿eli jest inny to sprobowaæ podmienic
			&& Game::getEnvironment()->cellInState(unit->getMainCell(), {CellState::COLLECT, CellState::EMPTY})
			&& Game::getEnvironment()->belowCellLimit(unit->getMainCell())
			&& parameter.thingsToInteract[0]->belowCloseLimit() > 0;
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->velocity = Urho3D::Vector2::ZERO;
		unit->currentFrameState = 0;

		unit->thingsToInteract.clear();
		unit->thingsToInteract.push_back(parameter.thingsToInteract[0]);
		unit->indexToInteract = parameter.index;

		unit->thingsToInteract[0]->upClose();

		Game::getEnvironment()->updateCell(unit->getMainCell(), 1, CellState::COLLECT);
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceClose();
			Game::getEnvironment()->updateCell(unit->getMainCell(), -1, CellState::NONE);
		}
		unit->indexToInteract = -1;
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);
		if (unit->isFirstThingAlive()
			&& unit->getMainCell() == unit->indexToInteract
			&& Game::getEnvironment()->cellInState(unit->getMainCell(), {CellState::COLLECT})) {
			//TODO musi byc dokladnie w dobry mbuckecie
			auto& resources = Game::getPlayersMan()->getPlayer(unit->player)->getResources();
			auto resource = dynamic_cast<ResourceEntity*>(unit->thingsToInteract[0]);
			const float value = resource->collect(unit->dbLevel->collectSpeed * timeStep);
			resources.add(resource->getId(), value);
		} else {
			StateManager::changeState(unit, UnitState::STOP);
		}
	}
};
