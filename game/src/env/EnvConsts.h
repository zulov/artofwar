#pragma once
#include <initializer_list>

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
