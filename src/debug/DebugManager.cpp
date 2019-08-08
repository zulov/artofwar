#include "debug/DebugManager.h"
#include "Game.h"
#include "simulation/env/Environment.h"

void DebugManager::change(Urho3D::Input* input, Simulation* simulation) {
	if (input->GetKeyPress(Urho3D::KEY_F9)) {
		simColorMode = nextColorMode[static_cast<char>(simColorMode)];
		simulation->changeColorMode(simColorMode);
	}

	if (input->GetKeyPress(Urho3D::KEY_F10)) {
		environmentDebugMode = nextEnvMode[static_cast<char>(environmentDebugMode)];
	} else if (input->GetKeyPress(Urho3D::KEY_F11)) {
		environmentIndex++;
		environmentIndex = environmentIndex % MAX_ENV_INDEX;
	}
}

void DebugManager::draw() const {
	Game::getEnvironment()->drawDebug(environmentDebugMode,environmentIndex);
}
