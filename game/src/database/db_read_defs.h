#pragma once

#include "db_columns.h"
#include "db_utils.h"
#include "scene/load/RuntimeSaveData.h"

template <>
inline ConfigSaveData readRow<ConfigSaveData>(sqlite3_stmt* stmt, int) {
	return {asInt(stmt, ConfigCol::precision),
			asInt(stmt, ConfigCol::map),
			asInt(stmt, ConfigCol::size),
			asUI(stmt, ConfigCol::total_ticks),
			asBool(stmt, ConfigCol::rdn_present),
			{asUI(stmt, ConfigCol::rdn_seed),
				{asInt(stmt, ConfigCol::rdn_ai_idx), asInt(stmt, ConfigCol::rdn_resource_rotation_idx),
				 asInt(stmt, ConfigCol::rdn_collision_force_idx), asInt(stmt, ConfigCol::rdn_other_idx)},
				{asInt(stmt, ConfigCol::rdn_save_idx), asInt(stmt, ConfigCol::rdn_player_name_idx),
				 asInt(stmt, ConfigCol::rdn_resource_node_idx)}}};
}

template <>
inline UnitOrderSaveData readRow<UnitOrderSaveData>(sqlite3_stmt* stmt, int precision) {
	return {asUI(stmt, UnitOrderCol::unit_uid), asByte(stmt, UnitOrderCol::action), asBool(stmt, UnitOrderCol::append),
			asBool(stmt, UnitOrderCol::has_target), asUI(stmt, UnitOrderCol::target_uid),
			asScaledFloat(stmt, UnitOrderCol::x, precision), asScaledFloat(stmt, UnitOrderCol::z, precision)};
}

template <>
inline UnitOrderRow readRow<UnitOrderRow>(sqlite3_stmt* stmt, int precision) {
	return {readRow<UnitOrderSaveData>(stmt, precision), asUShort(stmt, UnitOrderCol::order_idx)};
}

template <>
inline AimPathSaveData readRow<AimPathSaveData>(sqlite3_stmt* stmt, int) {
	return {asUI(stmt, AimPathCol::unit_uid), asText(stmt, AimPathCol::path), asText(stmt, AimPathCol::pending_path)};
}

template <>
inline QueueRow readRow<QueueRow>(sqlite3_stmt* stmt, int) {
	return {{asUI(stmt, QueueCol::owner_id), asUByte(stmt, QueueCol::owner_type), asByte(stmt, QueueCol::type),
				 asUShort(stmt, QueueCol::id), asUShort(stmt, QueueCol::level_id), asUShort(stmt, QueueCol::amount),
				 asUShort(stmt, QueueCol::elapsed_ticks)},
			asUShort(stmt, QueueCol::order_idx)};
}

template <>
inline PlayerLevelSaveData readRow<PlayerLevelSaveData>(sqlite3_stmt* stmt, int) {
	return {asUByte(stmt, PlayerLevelCol::player), asUByte(stmt, PlayerLevelCol::type),
			asUShort(stmt, PlayerLevelCol::id), asByte(stmt, PlayerLevelCol::level)};
}

template <>
inline AiSaveData readRow<AiSaveData>(sqlite3_stmt* stmt, int precision) {
	AiSaveData state;
	state.player = asUByte(stmt, AiStateCol::player);
	state.prevScore = asInt(stmt, AiStateCol::prev_score);
	state.prevEnemyScore = asInt(stmt, AiStateCol::prev_enemy_score);
	state.prevUnits = asUI(stmt, AiStateCol::prev_units);
	state.prevResSum = asScaledFloat(stmt, AiStateCol::prev_res_sum, precision);
	state.prevGatherSum = asScaledFloat(stmt, AiStateCol::prev_gather_sum, precision);
	state.foodPriority = asScaledFloat(stmt, AiStateCol::food_priority, precision);
	state.woodPriority = asScaledFloat(stmt, AiStateCol::wood_priority, precision);
	state.stonePriority = asScaledFloat(stmt, AiStateCol::stone_priority, precision);
	state.goldPriority = asScaledFloat(stmt, AiStateCol::gold_priority, precision);
	state.militaryPressure = {asScaledFloat(stmt, AiStateCol::pressure_our_army_our_econ, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_army_our_building, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_army_enemy_army, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_army_enemy_econ, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_army_enemy_building, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_army_battle, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_econ_our_building, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_econ_enemy_army, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_econ_enemy_econ, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_econ_enemy_building, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_econ_battle, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_building_enemy_army, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_building_enemy_econ, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_building_enemy_building, precision),
			asScaledFloat(stmt, AiStateCol::pressure_our_building_battle, precision),
			asScaledFloat(stmt, AiStateCol::pressure_enemy_army_enemy_econ, precision),
			asScaledFloat(stmt, AiStateCol::pressure_enemy_army_enemy_building, precision),
			asScaledFloat(stmt, AiStateCol::pressure_enemy_army_battle, precision),
			asScaledFloat(stmt, AiStateCol::pressure_enemy_econ_enemy_building, precision),
			asScaledFloat(stmt, AiStateCol::pressure_enemy_econ_battle, precision),
			asScaledFloat(stmt, AiStateCol::pressure_enemy_building_battle, precision)};
	state.lackingPerResource = {asScaledFloat(stmt, AiStateCol::lacking_food, precision),
			asScaledFloat(stmt, AiStateCol::lacking_wood, precision),
			asScaledFloat(stmt, AiStateCol::lacking_stone, precision), asScaledFloat(stmt, AiStateCol::lacking_gold, precision)};
	return state;
}

template <>
inline AiWantRow readRow<AiWantRow>(sqlite3_stmt* stmt, int precision) {
	return {{asUByte(stmt, AiWantCol::player), asScaledFloat(stmt, AiWantCol::priority, precision),
				asScaledFloat(stmt, AiWantCol::base_priority, precision), asUByte(stmt, AiWantCol::type), asUByte(stmt, AiWantCol::count),
				asShort(stmt, AiWantCol::specific_id), asUShort(stmt, AiWantCol::age),
				asUByte(stmt, AiWantCol::reserve_ticks), asBool(stmt, AiWantCol::active)},
			asUShort(stmt, AiWantCol::order_idx)};
}

template <>
inline AiHistoryRow readRow<AiHistoryRow>(sqlite3_stmt* stmt, int) {
	return {{asUByte(stmt, AiHistoryCol::player), asBool(stmt, AiHistoryCol::action), asUI(stmt, AiHistoryCol::tick),
				 asByte(stmt, AiHistoryCol::type), asByte(stmt, AiHistoryCol::result), asUI(stmt, AiHistoryCol::chosen_id)},
			asUShort(stmt, AiHistoryCol::order_idx)};
}

template <>
inline ProjectileSaveData readRow<ProjectileSaveData>(sqlite3_stmt* stmt, int precision) {
	return {asUI(stmt, ProjectileCol::aim_uid), asScaledFloat(stmt, ProjectileCol::percent_to_go, precision),
			asScaledFloat(stmt, ProjectileCol::speed, precision), asScaledFloat(stmt, ProjectileCol::attack_val, precision),
			asByte(stmt, ProjectileCol::player)};
}

template <>
inline FormationSaveData readRow<FormationSaveData>(sqlite3_stmt* stmt, int precision) {
	return {asShort(stmt, FormationCol::id), asByte(stmt, FormationCol::state), asByte(stmt, FormationCol::type),
			asScaledFloat(stmt, FormationCol::direction_x, precision), asScaledFloat(stmt, FormationCol::direction_z, precision)};
}

template <>
inline FormationOrderRow readRow<FormationOrderRow>(sqlite3_stmt* stmt, int precision) {
	return {{asShort(stmt, FormationOrderCol::formation_id), asShort(stmt, FormationOrderCol::action),
				asBool(stmt, FormationOrderCol::append), asBool(stmt, FormationOrderCol::has_target),
				asUI(stmt, FormationOrderCol::target_uid), asScaledFloat(stmt, FormationOrderCol::x, precision),
				asScaledFloat(stmt, FormationOrderCol::z, precision)},
			asUShort(stmt, FormationOrderCol::order_idx), asBool(stmt, FormationOrderCol::pending)};
}

template <>
inline PendingCommandSaveData readRow<PendingCommandSaveData>(sqlite3_stmt* stmt, int precision) {
	return {asUShort(stmt, PendingCommandCol::order_idx),
			static_cast<PendingCommandKind>(asByte(stmt, PendingCommandCol::kind)), asByte(stmt, PendingCommandCol::action),
			asByte(stmt, PendingCommandCol::action_type), asUShort(stmt, PendingCommandCol::id_db),
			asByte(stmt, PendingCommandCol::player), asByte(stmt, PendingCommandCol::level),
			asUI(stmt, PendingCommandCol::number), asScaledFloat(stmt, PendingCommandCol::x, precision),
			asScaledFloat(stmt, PendingCommandCol::z, precision), asScaledFloat(stmt, PendingCommandCol::hp, precision),
			asUI(stmt, PendingCommandCol::target_uid),
			asShort(stmt, PendingCommandCol::formation_id), asBool(stmt, PendingCommandCol::append), {}};
}

template <>
inline PendingCommandEntityRow readRow<PendingCommandEntityRow>(sqlite3_stmt* stmt, int) {
	return {asUShort(stmt, PendingCommandEntityCol::command_idx), asUShort(stmt, PendingCommandEntityCol::order_idx),
			asUI(stmt, PendingCommandEntityCol::uid)};
}
