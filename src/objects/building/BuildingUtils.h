#pragma once
#include "Game.h"
#include "database/DatabaseCache.h"
#include "database/db_strcut.h"
#include <unordered_set>


inline std::unordered_set<int> unitsIdsForBuildingNation(int nation, int i) {
	std::unordered_set<int> common;
	std::vector<db_unit*>* units = Game::getDatabaseCache()->getUnitsForBuilding(i);
	for (auto& unit : *units) {
		//todo to zrobic raz i pobierac
		if (unit->nation == nation) {
			common.insert(unit->id);
		}
	}
	return common;
}


inline std::unordered_set<int> pathsIdsInbuilding(int id) {
	std::unordered_set<int> common;
	auto paths = Game::getDatabaseCache()->getBuilding(id)->unitUpgradePath;
	for (auto id : paths) {
		common.insert(id);
	}
	return common;
}
