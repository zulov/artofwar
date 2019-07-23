#include "InfluenceManager.h"
#include "objects/unit/Unit.h"
#include "simulation/env/EnvConsts.h"
#include "simulation/env/ContentInfo.h"
#include "objects/CellState.h"
#include "Game.h"
#include "database/DatabaseCache.h"

InfluenceManager::InfluenceManager(char numberOfPlayers) {
	for (int i = 0; i < numberOfPlayers; ++i) {
		unitsNumberPerPlayer.emplace_back(new InfluenceMapInt(DEFAULT_INF_GRID_SIZE,BUCKET_GRID_SIZE));
	}
	ci = new content_info();
}

InfluenceManager::~InfluenceManager() {
	for (auto map : unitsNumberPerPlayer) {
		delete map;
	}
	delete ci;
}

void InfluenceManager::update(std::vector<Unit*>* units) const {
	for (auto map : unitsNumberPerPlayer) {
		map->reset();
	}
	for (auto unit : (*units)) {
		unitsNumberPerPlayer[unit->getPlayer()]->update(unit);
	}
}


content_info* InfluenceManager::getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfo,
                                               bool checks[], int activePlayer) {
	ci->reset();
	switch (state) {
	case CellState::EMPTY:
	case CellState::ATTACK:
	case CellState::COLLECT:
	case CellState::NONE:
		if (checks[3] || checks[4]) {
			for (int i = 0; i < unitsNumberPerPlayer.size(); ++i) {
				char value = unitsNumberPerPlayer[i]->getValue(center);
				if ((checks[3] && i == activePlayer || checks[4] && i != activePlayer) && value > 0) {
					ci->unitsNumberPerPlayer[i] = value;
					ci->hasUnit = true;
				}
			}
		}
		break;
	case CellState::RESOURCE:
		if (checks[1]) {
			ci->hasResource = true;
			ci->resourceNumber[additionalInfo]++;
		}
		break;
	case CellState::BUILDING:
		if (checks[2]) {
			ci->hasBuilding = true;
			ci->buildingNumberPerPlayer[additionalInfo]++;
		}
		break;
	default: ;
	}

	return ci;
}
