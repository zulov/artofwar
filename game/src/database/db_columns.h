#pragma once
#include <magic_enum.hpp>
#include <string>
#include <vector>

// --- Save/Load tables (scene) ---

enum class UnitCol : unsigned char {
	id_db,
	hp_coef,
	uid,
	player,
	level,
	position_x,
	position_z,
	state,
	velocity_x,
	velocity_z,
	next_state,
	state_pending,
	frame_state,
	formation,
	pos_in_state,
	command_priority,
	command_center,
	charge_energy,
	target_uid,
	pending_target_uid,
	aim_kind,
	aim_target_uid,
	aim_current,
	aim_direction_x,
	aim_direction_z,
	pending_aim_kind,
	pending_aim_target_uid,
	pending_aim_current,
	pending_aim_direction_x,
	pending_aim_direction_z
};

enum class BuildingCol : unsigned char {
	id_db,
	hp_coef,
	uid,
	player,
	level,
	bucket_x,
	bucket_y,
	state,
	next_state,
	target_uid,
	frame_state
};

enum class ResourceCol : unsigned char { id_db, hp_coef, uid, bucket_x, bucket_y, state, next_state };

enum class PlayerCol : unsigned char {
	id,
	is_active,
	team,
	nation,
	buildingUid,
	unitUid,
	food,
	wood,
	stone,
	gold,
	gather_food,
	gather_wood,
	gather_stone,
	gather_gold,
	pending_gather_food,
	pending_gather_wood,
	pending_gather_stone,
	pending_gather_gold,
	sum_food,
	sum_wood,
	sum_stone,
	sum_gold
};

enum class ConfigCol : unsigned char {
	precision,
	map,
	size,
	total_ticks,
	random_present,
	random_seed,
	random_float_ai_index,
	random_float_resource_rotation_index,
	random_float_collision_force_index,
	random_float_other_index,
	random_int_save_index,
	random_int_player_name_index,
	random_int_resource_node_index
};

enum class UnitOrderCol : unsigned char { unit_uid, order_idx, action, append, has_target, target_uid, x, z };

enum class AimPathCol : unsigned char { unit_uid, pending, order_idx, cell };

enum class QueueCol : unsigned char {
	owner_id,
	owner_type,
	order_idx,
	type,
	id,
	level_id,
	amount,
	elapsed_ticks
};

enum class PlayerLevelCol : unsigned char { player, type, id, level };

enum class AiStateCol : unsigned char {
	player,
	prev_score,
	prev_enemy_score,
	prev_units,
	prev_res_sum,
	prev_gather_sum,
	food_priority,
	wood_priority,
	stone_priority,
	gold_priority,
	pressure_our_army_our_econ,
	pressure_our_army_our_building,
	pressure_our_army_enemy_army,
	pressure_our_army_enemy_econ,
	pressure_our_army_enemy_building,
	pressure_our_army_battle,
	pressure_our_econ_our_building,
	pressure_our_econ_enemy_army,
	pressure_our_econ_enemy_econ,
	pressure_our_econ_enemy_building,
	pressure_our_econ_battle,
	pressure_our_building_enemy_army,
	pressure_our_building_enemy_econ,
	pressure_our_building_enemy_building,
	pressure_our_building_battle,
	pressure_enemy_army_enemy_econ,
	pressure_enemy_army_enemy_building,
	pressure_enemy_army_battle,
	pressure_enemy_econ_enemy_building,
	pressure_enemy_econ_battle,
	pressure_enemy_building_battle,
	lacking_food,
	lacking_wood,
	lacking_stone,
	lacking_gold
};

enum class AiWantCol : unsigned char {
	player,
	order_idx,
	priority,
	base_priority,
	type,
	count,
	specific_id,
	age,
	reserve_ticks,
	active
};

enum class AiHistoryCol : unsigned char { player, action, order_idx, tick, type, result, chosen_id };

enum class ProjectileCol : unsigned char {
	aim_uid,
	percent_to_go,
	speed,
	attack_val,
	player
};

enum class FormationCol : unsigned char { id, state, type, direction_x, direction_z };

enum class FormationOrderCol : unsigned char {
	formation_id,
	order_idx,
	pending,
	action,
	append,
	has_target,
	target_uid,
	x,
	z
};

enum class PendingCommandCol : unsigned char {
	order_idx,
	kind,
	action,
	action_type,
	id_db,
	player,
	level,
	number,
	x,
	z,
	hp,
	target_uid,
	formation_id,
	append
};

enum class PendingCommandEntityCol : unsigned char { command_idx, order_idx, uid };

// --- Data tables (data.db) ---

enum class DbUnitCol : unsigned char {
	id,
	name,
	icon,
	food,
	wood,
	stone,
	gold,
	action_state,
	type_infantry,
	type_range,
	type_cavalry,
	type_worker,
	type_special,
	type_melee,
	type_heavy,
	type_light
};

enum class DbUnitLevelCol : unsigned char {
	id,
	level,
	unit,
	name,
	node,
	food,
	wood,
	stone,
	gold,
	build_time,
	upgrade_time,
	min_dist,
	mass,
	min_speed,
	max_speed,
	max_force,
	max_hp,
	armor,
	sight_range,
	collect,
	attack,
	attack_reload,
	attack_range,
	bonus_infantry,
	bonus_range,
	bonus_cavalry,
	bonus_worker,
	bonus_special,
	bonus_melee,
	bonus_heavy,
	bonus_light,
	bonus_building
};

enum class DbBuildingCol : unsigned char {
	id,
	name,
	icon,
	food,
	wood,
	stone,
	gold,
	size_x,
	size_z,
	type_center,
	type_home,
	type_defence,
	type_resource,
	type_tech_blacksmith,
	type_tech_university,
	type_unit_barracks,
	type_unit_range,
	type_unit_cavalry,
	ruinable,
	to_resource
};

enum class DbBuildingLevelCol : unsigned char {
	id,
	level,
	building,
	name,
	node_name,
	food,
	wood,
	stone,
	gold,
	queue_max_capacity,
	build_speed,
	upgrade_speed,
	max_hp,
	armor,
	sight_range,
	collect,
	attack,
	attack_reload,
	attack_range,
	resource_range,
	food_storage,
	gold_storage,
	stone_refine_capacity,
	gold_refine_capacity,
	spawn_resource_time,
	spawn_resource_range
};

enum class DbNationCol : unsigned char { id, name, brain_prefix };

enum class DbResourceCol : unsigned char {
	id,
	resource_id,
	name,
	icon,
	max_hp,
	node_name,
	size_x,
	size_z,
	max_users,
	mini_map_color,
	collect_speed,
	rotatable
};

// --- Base.db tables ---

enum class HudSizeCol : unsigned char { id, name };

enum class SettingsCol : unsigned char { graph, resolution };

enum class ResolutionCol : unsigned char { id, x, y };

enum class GraphSettingsCol : unsigned char {
	id,
	hud_size,
	styles,
	fullscreen,
	max_fps,
	min_fps,
	name,
	v_sync,
	shadow,
	texture_quality
};

enum class HudVarsCol : unsigned char { id, hud_size, name, value };

// --- Map table ---

enum class MapCol : unsigned char { id, xml_name, name };

// --- Other data tables ---

enum class PlayerColorsCol : unsigned char { id, unit, building, name };

// --- Utility: generate column name vectors from enums ---

template <typename E>
std::vector<std::string> columnNames() {
	std::vector<std::string> result;
	result.reserve(magic_enum::enum_count<E>());
	for (auto e : magic_enum::enum_values<E>()) {
		result.emplace_back(magic_enum::enum_name(e));
	}
	return result;
}

// --- Save/Load contract checks ---
static_assert(magic_enum::enum_count<PlayerCol>() == 22,
			  "PlayerCol must match the 22 columns in SQLConsts::PLAYER_COL and bindRow<Player>");
static_assert(magic_enum::enum_count<UnitCol>() == 30,
			  "UnitCol must match the 30 columns in SQLConsts::UNIT_COL and bindRow<Unit>");
static_assert(magic_enum::enum_count<BuildingCol>() == 11,
			  "BuildingCol must match the 11 columns in SQLConsts::BUILDING_COL and bindRow<Building>");
static_assert(magic_enum::enum_count<ResourceCol>() == 7,
			  "ResourceCol must match the 7 columns in SQLConsts::RESOURCE_COL and bindRow<ResourceEntity>");
static_assert(magic_enum::enum_count<ConfigCol>() == 13,
			  "ConfigCol must match the 13 columns in SQLConsts::CONFIG_COL");
static_assert(magic_enum::enum_count<UnitOrderCol>() == 8,
			  "UnitOrderCol must match the 8 columns in SQLConsts::UNIT_ORDER_COL");
static_assert(magic_enum::enum_count<AimPathCol>() == 4,
			  "AimPathCol must match the 4 columns in SQLConsts::AIM_PATH_COL");
static_assert(magic_enum::enum_count<QueueCol>() == 8, "QueueCol must match the 8 columns in SQLConsts::QUEUE_COL");
static_assert(magic_enum::enum_count<PlayerLevelCol>() == 4,
			  "PlayerLevelCol must match the 4 columns in SQLConsts::PLAYER_LEVEL_COL");
static_assert(magic_enum::enum_count<AiStateCol>() == 35,
			  "AiStateCol must match the 35 columns in SQLConsts::AI_STATE_COL");
static_assert(magic_enum::enum_count<AiWantCol>() == 10,
			  "AiWantCol must match the 10 columns in SQLConsts::AI_WANT_COL");
static_assert(magic_enum::enum_count<AiHistoryCol>() == 7,
			  "AiHistoryCol must match the 7 columns in SQLConsts::AI_HISTORY_COL");
static_assert(magic_enum::enum_count<ProjectileCol>() == 5,
			  "ProjectileCol must match the 5 columns in SQLConsts::PROJECTILE_COL");
static_assert(magic_enum::enum_count<FormationCol>() == 5,
			  "FormationCol must match the 5 columns in SQLConsts::FORMATION_COL");
static_assert(magic_enum::enum_count<FormationOrderCol>() == 9,
			  "FormationOrderCol must match the 9 columns in SQLConsts::FORMATION_ORDER_COL");
static_assert(magic_enum::enum_count<PendingCommandCol>() == 14,
			  "PendingCommandCol must match the 14 columns in SQLConsts::PENDING_COMMAND_COL");
static_assert(magic_enum::enum_count<PendingCommandEntityCol>() == 3,
			  "PendingCommandEntityCol must match the 3 columns in SQLConsts::PENDING_COMMAND_ENTITY_COL");
