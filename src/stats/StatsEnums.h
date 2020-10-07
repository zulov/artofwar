#pragma once
enum class BasicInputType : char {
	RESULT =0,
	UNIT_NUMBER,
	BUILDING_NUMBER
};

enum class StatsOutputType : char {
	IDLE =0,

	CREATE_UNIT,
	CREATE_BUILDING,

	LEVEL_UP_BUILDING,
	LEVEL_UP_UNIT
};

enum class ResourceInputType : char {
	GOLD_SPEED,
	WOOD_SPEED,
	FOOD_SPEED,
	STONE_SPEED,
	GOLD_VALUE,
	WOOD_VALUE,
	FOOD_VALUE,
	STONE_VALUE,
	WORKERS	
};

template <typename T>
inline char cast(T type) {
	return static_cast<char>(type);
}
