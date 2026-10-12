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

#define SAVE_TABLE(COL, NAME, WITHOUT_ROWID, ...)                                                                        \
	template <>                                                                                                           \
	struct SaveTable<COL> {                                                                                                \
		inline static constexpr const char* name = NAME;                                                                    \
		static std::string schema() { return SQLConsts::makeSchema<COL>({__VA_ARGS__}, WITHOUT_ROWID); }                     \
	}

SAVE_TABLE(UnitCol, "units", true, UnitCol::uid);
SAVE_TABLE(BuildingCol, "buildings", true, BuildingCol::uid);
SAVE_TABLE(ResourceCol, "resources", true, ResourceCol::uid);
SAVE_TABLE(PlayerCol, "players", true, PlayerCol::id);
SAVE_TABLE(ConfigCol, "config", false);
SAVE_TABLE(UnitOrderCol, "unit_orders", true, UnitOrderCol::unit_uid, UnitOrderCol::order_idx);
SAVE_TABLE(AimPathCol, "aim_paths", true, AimPathCol::unit_uid);
SAVE_TABLE(QueueCol, "queues", true, QueueCol::owner_id, QueueCol::owner_type, QueueCol::order_idx);
SAVE_TABLE(PlayerLevelCol, "player_levels", true, PlayerLevelCol::player, PlayerLevelCol::type, PlayerLevelCol::id);
SAVE_TABLE(PlayerTechnologyCol, "player_technologies", true, PlayerTechnologyCol::player,
		   PlayerTechnologyCol::technology);
SAVE_TABLE(AiStateCol, "ai_state", true, AiStateCol::player);
SAVE_TABLE(AiWantCol, "ai_wants", true, AiWantCol::player, AiWantCol::order_idx);
SAVE_TABLE(AiHistoryCol, "ai_history", true, AiHistoryCol::player, AiHistoryCol::action, AiHistoryCol::order_idx);
SAVE_TABLE(ProjectileCol, "projectiles", false);
SAVE_TABLE(FormationCol, "formations", true, FormationCol::id);
SAVE_TABLE(FormationOrderCol, "formation_orders", true, FormationOrderCol::formation_id,
		   FormationOrderCol::order_idx, FormationOrderCol::pending);
SAVE_TABLE(PendingCommandCol, "pending_commands", true, PendingCommandCol::order_idx);
SAVE_TABLE(PendingCommandEntityCol, "pending_command_entities", true, PendingCommandEntityCol::command_idx,
		   PendingCommandEntityCol::order_idx);
SAVE_TABLE(WorldAgeStateCol, "world_age", false);

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
