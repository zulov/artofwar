#include "InfluenceManager.h"
#include "objects/unit/Unit.h"
#include "simulation/env/EnvConsts.h"

InfluenceManager::InfluenceManager(char numberOfPlayers) {
	for (int i = 0; i < numberOfPlayers; ++i) {
		unitsNumberPerPlayer.emplace_back(new InfluenceMapInt(DEFAULT_INF_GRID_SIZE,BUCKET_GRID_SIZE));
	}
}

InfluenceManager::~InfluenceManager() {
	for (auto map : unitsNumberPerPlayer) {
		delete map;
	}
}

void InfluenceManager::update(std::vector<Unit*>* units) {
	for (auto map : unitsNumberPerPlayer) {
		map->reset();
	}
	for (auto unit : (*units)) {
		unitsNumberPerPlayer[unit->getPlayer()]->update(unit);
	}
}
