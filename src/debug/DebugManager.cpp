#include "debug/DebugManager.h"
#include "Game.h"
#include "simulation/env/Environment.h"

void DebugManager::change(Urho3D::Input* input, Simulation* simulation) {
	if (input->GetKeyPress(Urho3D::KEY_F9)) {
		switch (simColorMode) {
		case ColorMode::BASIC:
			simColorMode = ColorMode::VELOCITY;
			break;
		case ColorMode::VELOCITY:
			simColorMode = ColorMode::STATE;
			break;
		case ColorMode::STATE:
			simColorMode = ColorMode::FORMATION;
			break;
		case ColorMode::FORMATION:
			simColorMode = ColorMode::BASIC;
			break;
		default:
			simColorMode = ColorMode::BASIC;
		}
		simulation->changeColorMode(simColorMode);
	}

	if (input->GetKeyPress(Urho3D::KEY_F10)) {
		Game::getEnvironment()->switchDebugGrid();
	} else if (input->GetKeyPress(Urho3D::KEY_F11)) {
		Game::getEnvironment()->nextDebugGrid();
	}
}
