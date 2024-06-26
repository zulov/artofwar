#pragma once

#include <iostream>
#include "database/db_strcut.h"

constexpr bool LOG_ACTIONS_MAKER = false;

inline void logThree(int ids[3], float vals[3], float max) {
	if constexpr (LOG_ACTIONS_MAKER) {
		for (int i = 0; i < 3; ++i) {
			std::cout << ids[i] << "-" << (vals[i] / max * 100) << "%, ";
		}
		std::cout << std::endl;
	}
}

inline void logResult(int id, const bool result) {
	if constexpr (LOG_ACTIONS_MAKER) {
		std::cout << " -> " << id;
		if (result) {
			std::cout << "(DONE)" << std::endl;
		} else {
			std::cout << std::endl;
		}
	}
}

inline void logBuilding(db_building* building) {
	if constexpr (LOG_ACTIONS_MAKER) {
		std::cout << building->name.CString();
	}
}

inline void logBuildingLevel(db_building* const building, std::optional<db_building_level*> opt) {
	if constexpr (LOG_ACTIONS_MAKER) {
		std::cout << building->name.CString() << " " << opt.value()->name.CString();
	}
}

inline void logUnit(db_unit* unit) {
	if constexpr (LOG_ACTIONS_MAKER) {
		std::cout << unit->name.CString();
	}
}

inline void logUnitLevel(db_unit* unit, std::optional<db_unit_level*> opt) {
	if constexpr (LOG_ACTIONS_MAKER) {
		std::cout << unit->name.CString() << " " << opt.value()->name.CString();
	}
}
