#pragma once

#include <string>
#include <vector>

#include "SQLConsts.h"
#include "database/db_columns.h"

template <typename Col>
const std::vector<std::string>& saveColumns() {
	static const auto columns = columnNames<Col>();
	return columns;
}

template <typename Col>
struct SaveTable;

#define SAVE_TABLE(COL, NAME)                                                                                           \
	template <>                                                                                                           \
	struct SaveTable<COL> {                                                                                                \
		inline static constexpr const char* name = SQLConsts::NAME##_NAME;                                                 \
		inline static constexpr const char* schema = SQLConsts::NAME##_COL;                                               \
	}

SAVE_TABLE(UnitCol, UNIT);
SAVE_TABLE(BuildingCol, BUILDING);
SAVE_TABLE(ResourceCol, RESOURCE);
SAVE_TABLE(PlayerCol, PLAYER);
SAVE_TABLE(ConfigCol, CONFIG);
SAVE_TABLE(UnitOrderCol, UNIT_ORDER);
SAVE_TABLE(AimPathCol, AIM_PATH);
SAVE_TABLE(QueueCol, QUEUE);
SAVE_TABLE(PlayerLevelCol, PLAYER_LEVEL);
SAVE_TABLE(AiStateCol, AI_STATE);
SAVE_TABLE(AiWantCol, AI_WANT);
SAVE_TABLE(AiHistoryCol, AI_HISTORY);
	SAVE_TABLE(ProjectileCol, PROJECTILE);
	SAVE_TABLE(FormationCol, FORMATION);
	SAVE_TABLE(FormationOrderCol, FORMATION_ORDER);
	SAVE_TABLE(PendingCommandCol, PENDING_COMMAND);
SAVE_TABLE(PendingCommandEntityCol, PENDING_COMMAND_ENTITY);

#undef SAVE_TABLE

template <typename Col>
std::string saveSelectSql(const std::string& suffix = {}) {
	std::string sql = "SELECT ";
	const auto& columns = saveColumns<Col>();
	for (size_t i = 0; i < columns.size(); ++i) {
		sql += columns[i];
		if (i + 1 < columns.size()) { sql += ", "; }
	}
	sql += " FROM ";
	sql += SaveTable<Col>::name;
	sql += suffix;
	return sql;
}
