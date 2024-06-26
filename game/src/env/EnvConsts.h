#pragma once
#include <magic_enum.hpp>

constexpr float BUCKET_GRID_FIELD_SIZE = 2.f;

constexpr float BUCKET_GRID_FIELD_SIZE_BUILD = 8.f;
constexpr float BUCKET_GRID_FIELD_SIZE_ENEMY = 16.f;
constexpr float BUCKET_GRID_FIELD_SIZE_RESOURCE = 16.f;

constexpr inline InfluenceDataType ENV_TO_INF_MAP[magic_enum::enum_count<EnvironmentDebugMode>()] = {
	InfluenceDataType::NONE, InfluenceDataType::NONE, InfluenceDataType::UNITS_NUMBER_PER_PLAYER,
	InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER, InfluenceDataType::BUILDING_INFLUENCE_PER_PLAYER,
	InfluenceDataType::RESOURCE_INFLUENCE, InfluenceDataType::FOOD_SPEED, InfluenceDataType::WOOD_SPEED,
	InfluenceDataType::STONE_SPEED, InfluenceDataType::GOLD_SPEED, InfluenceDataType::ATTACK_SPEED,

	//InfluenceDataType::ECON_QUAD, InfluenceDataType::BUILDINGS_QUAD, InfluenceDataType::UNITS_QUAD,

	InfluenceDataType::VISIBILITY
};
