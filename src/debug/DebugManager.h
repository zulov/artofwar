#pragma once
#include <Urho3D/Input/Input.h>
#include "simulation/Simulation.h"
#include "objects/unit/ColorMode.h"
#include "debug/EnvironmentDebugMode.h"

class DebugManager {
public:
	void change(Urho3D::Input* input, Simulation* simulation);
private:
	ColorMode simColorMode = ColorMode::BASIC;
	EnvironmentDebugMode environmentDebugMode = EnvironmentDebugMode::NONE;
};
