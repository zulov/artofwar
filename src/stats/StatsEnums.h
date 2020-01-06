#pragma once
enum class StatsInputType : char {
	RESULT =0,
	GOLD,
	WOOD,
	FOOD,
	STONE,
	ATTACK,
	DEFENCE,
	BASE_TO_ENEMY,
	UNITS,
	BUILDINGS,
	WORKERS,
};

#define STATS_OUTPUT_SIZE 18

enum class StatsOutputType : char {
	IDLE =0,
	
	CREATE_UNIT_ATTACK,
	CREATE_UNIT_DEFENCE,
	CREATE_UNIT_ECON,

	CREATE_BUILDING_ATTACK,
	CREATE_BUILDING_DEFENCE,
	CREATE_BUILDING_ECON,

	UPGRADE_ATTACK,
	UPGRADE_DEFENCE,
	UPGRADE_ECON,

	ORDER_GO,
	ORDER_STOP,
	ORDER_CHARGE,
	ORDER_ATTACK,
	ORDER_DEAD,
	ORDER_DEFEND,
	ORDER_FOLLOW,
	ORDER_COLLECT
};

inline char cast(StatsInputType type) {
	return static_cast<char>(type);
}

inline char cast(StatsOutputType type) {
	return static_cast<char>(type);
}
