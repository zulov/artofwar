#include "debug/DebugManager.h"
#include <magic_enum.hpp>
#include "Game.h"
#include "simulation/env/Environment.h"

void DebugManager::change(Urho3D::Input* input, Simulation* simulation) {
	if (input->GetKeyPress(Urho3D::KEY_F9)) {
		simColorMode = nextColorMode[static_cast<char>(simColorMode)];
		simulation->changeColorMode(simColorMode);

		Game::getLog()->Write(0, "Debug Mode Simulation change to "
		                      + Urho3D::String(magic_enum::enum_name(simColorMode).data()));
	}

	if (input->GetKeyPress(Urho3D::KEY_F10)) {
		environmentDebugMode = nextEnvMode[static_cast<char>(environmentDebugMode)];
		Game::getLog()->Write(2, "Debug Mode Environment change to "
			 + Urho3D::String(magic_enum::enum_name(environmentDebugMode).data()));
		environmentIndex = 0;
	} else if (input->GetKeyPress(Urho3D::KEY_F11)) {
		environmentIndex++;
		environmentIndex = environmentIndex % MAX_ENV_INDEX;

		Game::getLog()->Write(2, "Debug Mode Environment index change to "
		                      + Urho3D::String((int)environmentIndex));
	}
}

void DebugManager::draw() const {
	Game::getEnvironment()->drawDebug(environmentDebugMode, environmentIndex);
}
