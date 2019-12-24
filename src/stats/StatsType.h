#pragma once
enum class StatsType : char {
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

inline char cast(StatsType type) {
	return static_cast<char>(type);
}
