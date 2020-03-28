#pragma once
#include "database/db_strcut.h"
#include <unordered_set>


inline std::unordered_set<int> unitsIdsForBuildingNation(db_building_level* level, int nation) {
	std::unordered_set<int> common; //todo to zrobic raz i pobierac

	for (auto& unit : level->allUnits) {
		//if (unit->nation == nation) {
		common.insert(unit->id);
		//}
	}
	return common;
}
