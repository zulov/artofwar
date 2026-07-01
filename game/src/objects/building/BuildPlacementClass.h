#pragma once
#include <magic_enum.hpp>

// Placement intent for a building, finer-grained than ParentBuildingType so the
// build-spatial brain can push each resource-building subtype toward a different
// area (bonus buildings near un-bonused nodes, storage/refine near the economy,
// spawners/converters into open space, etc.).
enum class BuildPlacementClass : unsigned char {
	OTHER = 0,
	DEFENCE,
	TECH,
	UNITS,
	RES_BONUS,
	RES_CONVERT,
	RES_SPAWNER,
	RES_STORAGE,
	RES_REFINE,
};

inline constexpr unsigned char BUILD_PLACEMENT_CLASS_COUNT =
		magic_enum::enum_count<BuildPlacementClass>();
