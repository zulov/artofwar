#pragma once
#include <vector>
#include <string>
#include <magic_enum.hpp>

// --- Save/Load tables (scene) ---

enum class UnitCol : int {
	id_db, hp_coef, uid, player, level,
	position_x, position_z, state, velocity_x, velocity_z
};

enum class BuildingCol : int {
	id_db, hp_coef, uid, player, level,
	bucket_x, bucket_y, state, next_state, deploy_Idx
};

enum class ResourceCol : int {
	id_db, hp_coef, uid,
	bucket_x, bucket_y, state, next_state
};

enum class PlayerCol : int {
	id, is_active, team, nation, name, color,
	buildingUid, unitUid, food, wood, stone, gold
};

enum class ConfigCol : int {
	precision, map, size
};

// --- Data tables (data.db) ---

enum class DbUnitCol : int {
	id, name, icon,
	food, wood, stone, gold,
	action_state,
	type_infantry, type_range, type_cavalry, type_worker,
	type_special, type_melee, type_heavy, type_light
};

enum class DbUnitLevelCol : int {
	id, level, unit, name, node,
	food, wood, stone, gold,
	build_time, upgrade_time, min_dist, mass, min_speed, max_speed,
	max_force, max_hp, armor, sight_range, collect, attack, attack_reload,
	attack_range, bonus_infantry, bonus_range, bonus_cavalry, bonus_worker,
	bonus_special, bonus_melee, bonus_heavy, bonus_light, bonus_building
};

enum class DbBuildingCol : int {
	id, name, icon,
	food, wood, stone, gold,
	size_x, size_z,
	type_center, type_home, type_defence, type_resource,
	type_tech_blacksmith, type_tech_university,
	type_unit_barracks, type_unit_range, type_unit_cavalry,
	ruinable, to_resource
};

enum class DbBuildingLevelCol : int {
	id, level, building, name, node_name,
	food, wood, stone, gold,
	queue_max_capacity, build_speed, upgrade_speed, max_hp,
	armor, sight_range, collect, attack,
	attack_reload, attack_range, resource_range,
	food_storage, gold_storage, stone_refine_capacity, gold_refine_capacity,
	spawn_resource_time, spawn_resource_range
};

enum class DbNationCol : int {
	id, name, action_prefix, order_prefix
};

enum class DbResourceCol : int {
	id, resource_id, name, icon, max_hp,
	node_name, size_x, size_z, max_users,
	mini_map_color, collect_speed, rotatable
};

// --- Base.db tables ---

enum class HudSizeCol : int {
	id, name
};

enum class SettingsCol : int {
	graph, resolution
};

enum class ResolutionCol : int {
	id, x, y
};

enum class GraphSettingsCol : int {
	id, hud_size, styles, fullscreen, max_fps, min_fps,
	name, v_sync, shadow, texture_quality
};

enum class HudVarsCol : int {
	id, hud_size, name, value
};

// --- Map table ---

enum class MapCol : int {
	id, xml_name, name
};

// --- Other data tables ---

enum class PlayerColorsCol : int {
	id, unit, building, name
};

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
