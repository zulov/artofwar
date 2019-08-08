#include "InfluenceManager.h"
#include "objects/unit/Unit.h"
#include "simulation/env/EnvConsts.h"
#include "simulation/env/ContentInfo.h"
#include "objects/CellState.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "objects/building/Building.h"
#include "debug/DebugLineRepo.h"


InfluenceManager::InfluenceManager(char numberOfPlayers) {
	for (int i = 0; i < numberOfPlayers; ++i) {
		unitsNumberPerPlayer.emplace_back(new InfluenceMapInt(DEFAULT_INF_GRID_SIZE,BUCKET_GRID_SIZE));
		buildingsInfluencePerPlayer.emplace_back(
			new InfluenceMapFloat(DEFAULT_INF_FLOAT_GRID_SIZE,BUCKET_GRID_SIZE, 0.5, 3));
		unitsInfluencePerPlayer.emplace_back(
			new InfluenceMapFloat(DEFAULT_INF_FLOAT_GRID_SIZE,BUCKET_GRID_SIZE, 0.5, 3));
	}
	ci = new content_info();
	DebugLineRepo::init(DebugLineType::INFLUANCE, MAX_DEBUG_PARTS_INFLUANCE);
}

InfluenceManager::~InfluenceManager() {
	clear_vector(unitsNumberPerPlayer);
	clear_vector(buildingsInfluencePerPlayer);
	clear_vector(unitsInfluencePerPlayer);
	delete ci;
}

void InfluenceManager::update(std::vector<Unit*>* units) const {
	for (auto map : unitsNumberPerPlayer) {
		map->reset();
	}
	for (auto map : unitsInfluencePerPlayer) {
		map->reset();
	}
	for (auto unit : (*units)) {
		unitsNumberPerPlayer[unit->getPlayer()]->update(unit);
		unitsInfluencePerPlayer[unit->getPlayer()]->update(unit);
	}
}

void InfluenceManager::update(std::vector<Building*>* buildings) const {
	for (auto map : buildingsInfluencePerPlayer) {
		map->reset();
	}
	for (auto building : (*buildings)) {
		buildingsInfluencePerPlayer[building->getPlayer()]->update(building);
	}
}


void InfluenceManager::draw(InfluanceType type, char index) {
	DebugLineRepo::clear(DebugLineType::INFLUANCE, currentDebugBatch);
	DebugLineRepo::beginGeometry(DebugLineType::INFLUANCE, currentDebugBatch);

	switch (type) {
	case InfluanceType::NONE:
		break;
	case InfluanceType::UNITS_NUMBER_PER_PLAYER:
		index = index % unitsNumberPerPlayer.size();
		unitsNumberPerPlayer[index]->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUANCE);
		break;
	case InfluanceType::UNITS_INFLUENCE_PER_PLAYER:
		index = index % unitsInfluencePerPlayer.size();
		unitsInfluencePerPlayer[index]->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUANCE);
		break;
	default: ;
	}
	DebugLineRepo::commit(DebugLineType::INFLUANCE, currentDebugBatch);
	currentDebugBatch++;
	if (currentDebugBatch >= MAX_DEBUG_PARTS_INFLUANCE) {
		currentDebugBatch = 0;
	}
}

void InfluenceManager::switchDebug() {
	switch (debugType) {
	case InfluanceType::NONE:
		debugType = InfluanceType::UNITS_NUMBER_PER_PLAYER;
		break;
	case InfluanceType::UNITS_NUMBER_PER_PLAYER:
		debugType = InfluanceType::UNITS_INFLUENCE_PER_PLAYER;
		break;
	case InfluanceType::UNITS_INFLUENCE_PER_PLAYER:
		debugType = InfluanceType::NONE;
		break;
	default: ;
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
