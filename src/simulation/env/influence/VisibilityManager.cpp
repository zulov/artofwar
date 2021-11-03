#include "VisibilityManager.h"

#include <iostream>
#include <Urho3D/Graphics/TerrainPatch.h>

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
	for (int player = 0; player < numberOfPlayers; ++player) {
		visibilityPerPlayer.emplace_back(new VisibilityMap(mapSize / VISIBILITY_GRID_FIELD_SIZE, mapSize, 3));
	}
	calculator = GridCalculatorProvider::get(mapSize / VISIBILITY_GRID_FIELD_SIZE, mapSize);
}

VisibilityManager::~VisibilityManager() {
	clear_vector(visibilityPerPlayer);
}

void VisibilityManager::updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units) const {
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
		auto a = Game::getColorPaletteRepo()->getTerrainColor(VisibilityType::NONE);
		auto b = Game::getColorPaletteRepo()->getTerrainColor(VisibilityType::SEEN);
		auto c = Game::getColorPaletteRepo()->getTerrainColor(VisibilityType::VISIBLE);

		for (int i = 0; i < current->getResolution() * current->getResolution(); ++i) {
			auto cords = calculator->getIndexes(i);
			auto patch = terrain->GetPatch(cords.x_, cords.y_);
			char val = current->getValueAt(i);

			if (val == static_cast<char>(VisibilityType::VISIBLE)) {
				patch->SetMaterial(c);
			} else if (val == static_cast<char>(VisibilityType::SEEN)) {
				patch->SetMaterial(b);
			} else if (val == static_cast<char>(VisibilityType::NONE)) {
				patch->SetMaterial(a);
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
