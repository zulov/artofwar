#pragma once
#include <initializer_list>

#include <magic_enum.hpp>

constexpr float BUCKET_GRID_FIELD_SIZE = 2.f;

constexpr float BUCKET_GRID_FIELD_SIZE_BUILD = 8.f;
constexpr float BUCKET_GRID_FIELD_SIZE_ENEMY = 16.f;
constexpr float BUCKET_GRID_FIELD_SIZE_RESOURCE = 16.f;

constexpr std::initializer_list<float> RESOURCE_GRID_QUERY_RADIUS_LEVELS = {64.f, 128.f, 256.f};

// Indexes into RESOURCE_GRID_QUERY_RADIUS_LEVELS; names denote the level's query radius.
enum class ResourceQueryLevel : int {
	R64 = 0,
	R128 = 1,
	R256 = 2,
};

constexpr inline InfluenceDataType ENV_TO_INF_MAP[magic_enum::enum_count<EnvironmentDebugMode>()] = {
	InfluenceDataType::NONE, InfluenceDataType::NONE, InfluenceDataType::UNITS_NUMBER_PER_PLAYER,
	InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER, InfluenceDataType::BUILDING_INFLUENCE_PER_PLAYER,
	InfluenceDataType::FOOD_SPEED, InfluenceDataType::WOOD_SPEED,
	InfluenceDataType::STONE_SPEED, InfluenceDataType::GOLD_SPEED, InfluenceDataType::ATTACK_SPEED,

	//InfluenceDataType::ECON_QUAD, InfluenceDataType::BUILDINGS_QUAD, InfluenceDataType::ARMY_QUAD,

	InfluenceDataType::VISIBILITY
};
