#include "CollectState.h"
#include "../Unit.h"
#include "player/Resources.h"
#include "player/PlayersManager.h"
#include "Game.h"


CollectState::CollectState() = default;


CollectState::~CollectState() = default;

void CollectState::onStart(Unit* unit) {
}

void CollectState::onStart(Unit* unit, ActionParameter& parameter) {
	//unit->resource = ...;
	unit->resource->up();
}

void CollectState::onEnd(Unit* unit) {
	State::onEnd(unit);
	unit->resource->reduce();
	unit->resource = nullptr;
	*unit->toResource = Vector2();
}

void CollectState::execute(Unit* unit) {
	State::execute(unit);
	Resources& resources = Game::get()->getPlayersManager()->getPlayer(unit->player)->getResources();

	if (unit->resource) {
		const double value = unit->resource->collect(unit->collectSpeed);
		resources.add(unit->resource->getDbID(), value);
		Vector3 a = *unit->resource->getPosition() - *unit->position;
		* unit->toResource = Vector2(a.x_, a.z_);
	}

}
