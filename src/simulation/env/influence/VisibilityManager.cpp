#include "VisibilityManager.h"

#include <iostream>

#include "Game.h"
#include "MapsUtils.h"
#include "colors/ColorPaletteRepo.h"
#include "map/VisibilityMap.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "utils/DeleteUtils.h"
#include "map/VisibilityType.h"
#include "player/PlayersManager.h"
#include "simulation/SimGlobals.h"
#include "simulation/env/Environment.h"
#include "simulation/env/GridCalculatorProvider.h"


VisibilityManager::VisibilityManager(char numberOfPlayers, float mapSize) {
	visibilityPerPlayer.reserve(numberOfPlayers);
	const unsigned short resolution = mapSize / VISIBILITY_GRID_FIELD_SIZE;

	for (int player = 0; player < numberOfPlayers; ++player) {
		visibilityPerPlayer.emplace_back(new VisibilityMap(resolution, mapSize, 3));
	}
	calculator = GridCalculatorProvider::get(resolution, mapSize);
}

VisibilityManager::~VisibilityManager() {
	clear_vector(visibilityPerPlayer);
}

void VisibilityManager::updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
                                         std::vector<ResourceEntity*>* resources) const {
	MapsUtils::resetMaps(visibilityPerPlayer);
	for (const auto unit : (*units)) {
		visibilityPerPlayer[unit->getPlayer()]->update(unit);
	}
	for (const auto building : (*buildings)) {
		visibilityPerPlayer[building->getPlayer()]->update(building);
	}

	const auto terrain = Game::getEnvironment()->getTerrain();
	if (terrain && !SIM_GLOBALS.HEADLESS) {
		auto current = visibilityPerPlayer[Game::getPlayersMan()->getActivePlayerID()];

		for (const auto resource : (*resources)) {
			auto pos = resource->getPosition();

			if (current->getValueAt(Urho3D::Vector2(pos.x_, pos.z_)) == static_cast<char>(VisibilityType::VISIBLE)) {
				resource->setVisibility(true);
			} else {
				resource->setVisibility(false);
			}
		}

	}
}

void VisibilityManager::drawMaps(short currentDebugBatch, char index) const {
	MapsUtils::drawMap(currentDebugBatch, index, visibilityPerPlayer);
}

bool VisibilityManager::isVisible(char player, const Urho3D::Vector2& pos) const {
	return visibilityPerPlayer[player]->getValueAt(pos) == static_cast<char>(VisibilityType::VISIBLE);
}

float VisibilityManager::getVisibilityScore(char player) {
	return visibilityPerPlayer[player]->getPercent();
}

void VisibilityManager::removeUnseen(char player, float* intersection) {
	visibilityPerPlayer[player]->removeUnseen(intersection);
}
