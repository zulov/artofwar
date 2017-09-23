#include "CollectState.h"
#include "../Unit.h"


CollectState::CollectState() {
}


CollectState::~CollectState() {
}

void CollectState::onStart(Unit* unit) {
}

void CollectState::onStart(Unit* unit, ActionParameter* parameter) {
	//unit->resource = ...;
	unit->resource->up();
}

void CollectState::onEnd(Unit* unit) {
	unit->resource->reduce();
	unit->resource = nullptr;
	(*unit->toResource) = Vector3();;
}

void CollectState::execute(Unit* unit) {
	Resources* resources = Game::get()->getPlayersManager()->getPlayer(unit->player)->getResources();

	if (unit->resource) {
		double value = unit->resource->collect(unit->collectSpeed);
		resources->add(unit->resource->getSubTypeId(), value);
		(*unit->toResource) = *unit->resource->getPosition() - *unit->position;
	}

}
