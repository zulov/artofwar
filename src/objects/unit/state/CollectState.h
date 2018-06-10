#pragma once
#include "Game.h"
#include "State.h"
#include "objects/resource/ResourceEntity.h"
#include "player/Resources.h"
#include <player/PlayersManager.h>


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
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->upClose();
		}
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		unit->resource->reduceClose();
		unit->resource = nullptr;
		*unit->toResource = Urho3D::Vector2();
	}

	void execute(Unit* unit) override {
		State::execute(unit);
		Resources& resources = Game::getPlayersManager()->getPlayer(unit->player)->getResources();

		if (unit->resource && unit->resource->isAlive()) {
			const float value = unit->resource->collect(unit->collectSpeed);
			resources.add(unit->resource->getDbID(), value);
			Vector3 a = *unit->resource->getPosition() - *unit->position;
			*unit->toResource = Vector2(a.x_, a.z_);
		} else {
			StateManager::changeState(unit, UnitState::STOP);
		}
	}
};
