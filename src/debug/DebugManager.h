#pragma once
#include <magic_enum.hpp>
#include "debug/EnvironmentDebugMode.h"
#include "objects/unit/SimColorMode.h"

namespace Urho3D {
	class Input;
}

class Simulation;
constexpr char MAX_ENV_INDEX = 32;

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
		EnvironmentDebugMode::INF_RESOURCE, EnvironmentDebugMode::NONE
	};
	SimColorMode nextColorMode[magic_enum::enum_count<SimColorMode>()] = {
		SimColorMode::VELOCITY, SimColorMode::STATE, SimColorMode::FORMATION, SimColorMode::BASIC
	};
};
