#pragma once
#include "database/db_strcut.h"

inline auto attackLevelValue = [](db_ai_prop_level* level) {
	return level->aiProps->attack;
};

inline auto econLevelValue = [](db_ai_prop_level* level) {
	return level->aiProps->econ;
};

inline auto defenceLevelValue = [](db_ai_prop_level* level) {
	return level->aiProps->defence;
};

inline auto attackLevelUpValue = [](db_ai_prop_level* level) {
	return level->aiPropsLevelUp->attack;
};

inline auto econLevelUpValue = [](db_ai_prop_level* level) {
	return level->aiPropsLevelUp->econ;
};

inline auto defenceLevelUpValue = [](db_ai_prop_level* level) {
	return level->aiPropsLevelUp->defence;
};