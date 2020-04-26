#pragma once
#include <Urho3D/Input/Input.h>
#include "simulation/Simulation.h"
#include "objects/unit/SimColorMode.h"
#include "debug/EnvironmentDebugMode.h"
#include <magic_enum.hpp>

#define MAX_ENV_INDEX 32;

class DebugManager {
public:
	void change(Urho3D::Input* input, Simulation* simulation);
	void draw() const;
private:
	SimColorMode simColorMode = SimColorMode::BASIC;
	EnvironmentDebugMode environmentDebugMode = EnvironmentDebugMode::NONE;
	char environmentIndex = 0;

	EnvironmentDebugMode nextEnvMode[magic_enum::enum_count<EnvironmentDebugMode>()] = {
		EnvironmentDebugMode::MAIN_GRID, EnvironmentDebugMode::INF_UNITS_NUMBER_PER_PLAYER,
		EnvironmentDebugMode::INF_UNITS_INFLUENCE_PER_PLAYER, EnvironmentDebugMode::INF_BUILDING_INFLUENCE_PER_PLAYER,
		EnvironmentDebugMode::INF_RESOURCE, EnvironmentDebugMode::INF_ATTACK_INFLUENCE_PER_PLAYER,
		EnvironmentDebugMode::INF_DEFENCE_INFLUENCE_PER_PLAYER, EnvironmentDebugMode::INF_ECON_INFLUENCE_PER_PLAYER,
		EnvironmentDebugMode::NONE
	};
	SimColorMode nextColorMode[magic_enum::enum_count<SimColorMode>()] = {
		SimColorMode::VELOCITY, SimColorMode::STATE, SimColorMode::FORMATION, SimColorMode::BASIC
	};
};
