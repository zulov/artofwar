#pragma once
#define ENVIRONMENT_DEBUG_SIZE 4

enum class EnvironmentDebugMode : char {
	NONE,
	MAIN_GRID,
	INF_UNITS_NUMBER_PER_PLAYER,
	INF_UNITS_INFLUENCE_PER_PLAYER
};
