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
			asBool(stmt, ConfigCol::random_present),
			{asUI(stmt, ConfigCol::random_seed),
				{asInt(stmt, ConfigCol::random_float_ai_index), asInt(stmt, ConfigCol::random_float_resource_rotation_index),
				 asInt(stmt, ConfigCol::random_float_collision_force_index), asInt(stmt, ConfigCol::random_float_other_index)},
				{asInt(stmt, ConfigCol::random_int_save_index), asInt(stmt, ConfigCol::random_int_player_name_index),
				 asInt(stmt, ConfigCol::random_int_resource_node_index)}}};
}

template <>
inline UnitOrderSaveData readRow<UnitOrderSaveData>(sqlite3_stmt* stmt, int) {
	return {asUI(stmt, UnitOrderCol::unit_uid), asByte(stmt, UnitOrderCol::action), asBool(stmt, UnitOrderCol::append),
			asBool(stmt, UnitOrderCol::has_target), asUI(stmt, UnitOrderCol::target_uid), asFloat(stmt, UnitOrderCol::x),
			asFloat(stmt, UnitOrderCol::z)};
}

template <>
inline UnitOrderRow readRow<UnitOrderRow>(sqlite3_stmt* stmt, int precision) {
	return {readRow<UnitOrderSaveData>(stmt, precision), asUShort(stmt, UnitOrderCol::order_idx)};
}

template <>
inline AimPathRow readRow<AimPathRow>(sqlite3_stmt* stmt, int) {
	return {asUI(stmt, AimPathCol::unit_uid), asBool(stmt, AimPathCol::pending),
			asUShort(stmt, AimPathCol::order_idx), asInt(stmt, AimPathCol::cell)};
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
inline AiSaveData readRow<AiSaveData>(sqlite3_stmt* stmt, int) {
	AiSaveData state;
	state.player = asUByte(stmt, AiStateCol::player);
	state.prevScore = asInt(stmt, AiStateCol::prev_score);
	state.prevEnemyScore = asInt(stmt, AiStateCol::prev_enemy_score);
	state.prevUnits = asUI(stmt, AiStateCol::prev_units);
	state.prevResSum = asFloat(stmt, AiStateCol::prev_res_sum);
	state.prevGatherSum = asFloat(stmt, AiStateCol::prev_gather_sum);
	state.foodPriority = asFloat(stmt, AiStateCol::food_priority);
	state.woodPriority = asFloat(stmt, AiStateCol::wood_priority);
	state.stonePriority = asFloat(stmt, AiStateCol::stone_priority);
	state.goldPriority = asFloat(stmt, AiStateCol::gold_priority);
	state.militaryPressure = {asFloat(stmt, AiStateCol::pressure_our_army_our_econ),
			asFloat(stmt, AiStateCol::pressure_our_army_our_building), asFloat(stmt, AiStateCol::pressure_our_army_enemy_army),
			asFloat(stmt, AiStateCol::pressure_our_army_enemy_econ), asFloat(stmt, AiStateCol::pressure_our_army_enemy_building),
			asFloat(stmt, AiStateCol::pressure_our_army_battle), asFloat(stmt, AiStateCol::pressure_our_econ_our_building),
			asFloat(stmt, AiStateCol::pressure_our_econ_enemy_army), asFloat(stmt, AiStateCol::pressure_our_econ_enemy_econ),
			asFloat(stmt, AiStateCol::pressure_our_econ_enemy_building), asFloat(stmt, AiStateCol::pressure_our_econ_battle),
			asFloat(stmt, AiStateCol::pressure_our_building_enemy_army),
			asFloat(stmt, AiStateCol::pressure_our_building_enemy_econ),
			asFloat(stmt, AiStateCol::pressure_our_building_enemy_building),
			asFloat(stmt, AiStateCol::pressure_our_building_battle),
			asFloat(stmt, AiStateCol::pressure_enemy_army_enemy_econ),
			asFloat(stmt, AiStateCol::pressure_enemy_army_enemy_building), asFloat(stmt, AiStateCol::pressure_enemy_army_battle),
			asFloat(stmt, AiStateCol::pressure_enemy_econ_enemy_building), asFloat(stmt, AiStateCol::pressure_enemy_econ_battle),
			asFloat(stmt, AiStateCol::pressure_enemy_building_battle)};
	state.lackingPerResource = {asFloat(stmt, AiStateCol::lacking_food), asFloat(stmt, AiStateCol::lacking_wood),
			asFloat(stmt, AiStateCol::lacking_stone), asFloat(stmt, AiStateCol::lacking_gold)};
	return state;
}

template <>
inline AiWantRow readRow<AiWantRow>(sqlite3_stmt* stmt, int) {
	return {{asUByte(stmt, AiWantCol::player), asFloat(stmt, AiWantCol::priority),
				asFloat(stmt, AiWantCol::base_priority), asUByte(stmt, AiWantCol::type), asUByte(stmt, AiWantCol::count),
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
inline ProjectileSaveData readRow<ProjectileSaveData>(sqlite3_stmt* stmt, int) {
	return {asUI(stmt, ProjectileCol::aim_uid), asFloat(stmt, ProjectileCol::percent_to_go),
			asFloat(stmt, ProjectileCol::speed), asFloat(stmt, ProjectileCol::attack_val), asByte(stmt, ProjectileCol::player)};
}

template <>
inline FormationSaveData readRow<FormationSaveData>(sqlite3_stmt* stmt, int) {
	return {asShort(stmt, FormationCol::id), asByte(stmt, FormationCol::state), asByte(stmt, FormationCol::type),
			asFloat(stmt, FormationCol::direction_x), asFloat(stmt, FormationCol::direction_z)};
}

template <>
inline FormationOrderRow readRow<FormationOrderRow>(sqlite3_stmt* stmt, int) {
	return {{asShort(stmt, FormationOrderCol::formation_id), asShort(stmt, FormationOrderCol::action),
				asBool(stmt, FormationOrderCol::append), asBool(stmt, FormationOrderCol::has_target),
				asUI(stmt, FormationOrderCol::target_uid), asFloat(stmt, FormationOrderCol::x), asFloat(stmt, FormationOrderCol::z)},
			asUShort(stmt, FormationOrderCol::order_idx), asBool(stmt, FormationOrderCol::pending)};
}

template <>
inline PendingCommandSaveData readRow<PendingCommandSaveData>(sqlite3_stmt* stmt, int) {
	return {asUShort(stmt, PendingCommandCol::order_idx),
			static_cast<PendingCommandKind>(asByte(stmt, PendingCommandCol::kind)), asByte(stmt, PendingCommandCol::action),
			asByte(stmt, PendingCommandCol::action_type), asUShort(stmt, PendingCommandCol::id_db),
			asByte(stmt, PendingCommandCol::player), asByte(stmt, PendingCommandCol::level),
			asUI(stmt, PendingCommandCol::number), asFloat(stmt, PendingCommandCol::x), asFloat(stmt, PendingCommandCol::z),
			asFloat(stmt, PendingCommandCol::hp), asUI(stmt, PendingCommandCol::target_uid),
			asShort(stmt, PendingCommandCol::formation_id), asBool(stmt, PendingCommandCol::append), {}};
}

template <>
inline PendingCommandEntityRow readRow<PendingCommandEntityRow>(sqlite3_stmt* stmt, int) {
	return {asUShort(stmt, PendingCommandEntityCol::command_idx), asUShort(stmt, PendingCommandEntityCol::order_idx),
			asUI(stmt, PendingCommandEntityCol::uid)};
}
