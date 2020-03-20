#pragma once
#include "Game.h"
#include "database/DatabaseCache.h"
#include "database/db_strcut.h"
#include <unordered_set>


inline std::unordered_set<int> unitsIdsForBuildingNation(int nation, int i) {
	std::unordered_set<int> common; //todo to zrobic raz i pobierac
	for (auto& unit : Game::getDatabase()->getBuilding(i)->units) {
		if (unit->nation == nation) {
			common.insert(unit->id);
		}
	}
	return common;
}
