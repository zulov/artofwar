#pragma once
#include "Game.h"
#include "State.h"
#include "objects/resource/ResourceEntity.h"
#include "player/Resources.h"
#include <player/PlayersManager.h>


class CollectState : public State
{
public:

	CollectState() {}
	~CollectState() = default;

	void onStart(Unit* unit, ActionParameter& parameter) override {
		//unit->resource = ...;
		unit->resource->up();
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		unit->resource->reduce();
		unit->resource = nullptr;
		*unit->toResource = Urho3D::Vector2();
	}

	void execute(Unit* unit) override {
		State::execute(unit);
		Resources& resources = Game::get()->getPlayersManager()->getPlayer(unit->player)->getResources();

		if (unit->resource) {
			const float value = unit->resource->collect(unit->collectSpeed);
			resources.add(unit->resource->getDbID(), value);
			Vector3 a = *unit->resource->getPosition() - *unit->position;
			* unit->toResource = Vector2(a.x_, a.z_);
		}

	}
};
