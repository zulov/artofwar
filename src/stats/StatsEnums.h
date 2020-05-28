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
	UNITS_ATTACK,
	UNITS_DEFENCE,
	UNITS_ECONOMY,
	BUILDINGS_ATTACK,
	BUILDINGS_DEFENCE,
	BUILDINGS_ECONOMY,
	WORKERS,
};

enum class StatsOutputType : char {
	IDLE =0,

	CREATE_UNIT,
	CREATE_BUILDING,

	LEVEL_UP_BUILDING,
	LEVEL_UP_UNIT
};

enum class StatsOrderOutputType : char {
	IDLE =0,
	
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

inline char cast(StatsOrderOutputType type) {
	return static_cast<char>(type);
}
