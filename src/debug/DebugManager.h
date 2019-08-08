#pragma once
#include <Urho3D/Input/Input.h>
#include "simulation/Simulation.h"
#include "objects/unit/ColorMode.h"
#include "debug/EnvironmentDebugMode.h"

#define MAX_ENV_INDEX 32;

class DebugManager {
public:
	void change(Urho3D::Input* input, Simulation* simulation);
	void draw() const;
private:
	ColorMode simColorMode = ColorMode::BASIC;
	EnvironmentDebugMode environmentDebugMode = EnvironmentDebugMode::NONE;
	char environmentIndex = 0;

	EnvironmentDebugMode nextEnvMode[ENVIRONMENT_DEBUG_SIZE] = {
		EnvironmentDebugMode::MAIN_GRID, EnvironmentDebugMode::INF_UNITS_NUMBER_PER_PLAYER,
		EnvironmentDebugMode::INF_UNITS_INFLUENCE_PER_PLAYER, EnvironmentDebugMode::NONE
	};
	ColorMode nextColorMode[COLOR_MODE_SIZE] = {
		ColorMode::VELOCITY, ColorMode::STATE, ColorMode::FORMATION, ColorMode::BASIC
	};
};
