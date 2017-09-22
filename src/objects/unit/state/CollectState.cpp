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
}

void CollectState::onEnd(Unit* unit) {
	unit->resource = nullptr;
}

void CollectState::execute(Unit* unit) {
	Resources* resources = Game::get()->getPlayersManager()->getPlayer(unit->player)->getResources();
	unit->collect();
}
