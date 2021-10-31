#include "VisibilityManager.h"

#include "MapsUtils.h"
#include "map/VisibilityMap.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "utils/DeleteUtils.h"
#include "map/VisibilityType.h"


VisibilityManager::VisibilityManager(char numberOfPlayers, float mapSize) {
	visibilityPerPlayer.reserve(numberOfPlayers);
	for (int player = 0; player < numberOfPlayers; ++player) {
		visibilityPerPlayer.emplace_back(new VisibilityMap(mapSize / VISIBILITY_GRID_FIELD_SIZE, mapSize, 3));
	}
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
