#include "InfluenceManager.h"
#include "objects/unit/Unit.h"
#include "simulation/env/EnvConsts.h"
#include "simulation/env/ContentInfo.h"
#include "objects/CellState.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "objects/building/Building.h"
#include "debug/DebugLineRepo.h"
#include "objects/ValueType.h"
#include "player/ai/AiInfluenceType.h"


InfluenceManager::InfluenceManager(char numberOfPlayers) {
	for (int i = 0; i < numberOfPlayers; ++i) {
		unitsNumberPerPlayer.emplace_back(new InfluenceMapInt(DEFAULT_INF_GRID_SIZE, BUCKET_GRID_SIZE, 40));
		buildingsInfluencePerPlayer.emplace_back(
			new InfluenceMapFloat(DEFAULT_INF_FLOAT_GRID_SIZE, BUCKET_GRID_SIZE, 0.5, 2, 5));
		unitsInfluencePerPlayer.emplace_back(
			new InfluenceMapFloat(DEFAULT_INF_FLOAT_GRID_SIZE, BUCKET_GRID_SIZE, 0.5, 2, 40));
		attackLevelPerPlayer.emplace_back(
			new InfluenceMapFloat(DEFAULT_INF_FLOAT_GRID_SIZE, BUCKET_GRID_SIZE, 0.5, 2, 40));
		defenceLevelPerPlayer.emplace_back(
			new InfluenceMapFloat(DEFAULT_INF_FLOAT_GRID_SIZE, BUCKET_GRID_SIZE, 0.5, 2, 40));
		econLevelPerPlayer.emplace_back(
			new InfluenceMapFloat(DEFAULT_INF_FLOAT_GRID_SIZE, BUCKET_GRID_SIZE, 0.5, 2, 40));
	}

	for (int i = 0; i < Game::getDatabase()->getResourceSize(); ++i) {
		resourceInfluence.
			emplace_back(new InfluenceMapFloat(DEFAULT_INF_FLOAT_GRID_SIZE, BUCKET_GRID_SIZE, 0.5, 2, 40));
	}

	ci = new content_info();
	DebugLineRepo::init(DebugLineType::INFLUANCE, MAX_DEBUG_PARTS_INFLUENCE);
}

InfluenceManager::~InfluenceManager() {
	clear_vector(unitsNumberPerPlayer);
	clear_vector(buildingsInfluencePerPlayer);
	clear_vector(unitsInfluencePerPlayer);
	clear_vector(defenceLevelPerPlayer);
	clear_vector(attackLevelPerPlayer);
	clear_vector(econLevelPerPlayer);
	delete ci;
}

void InfluenceManager::update(std::vector<Unit*>* units) const {
	resetMapsI(unitsNumberPerPlayer);
	resetMapsF(unitsInfluencePerPlayer);

	for (auto unit : (*units)) {
		unitsNumberPerPlayer[unit->getPlayer()]->update(unit);
		unitsInfluencePerPlayer[unit->getPlayer()]->update(unit);
	}
	calcStats(unitsNumberPerPlayer);
	calcStats(unitsInfluencePerPlayer);
}

void InfluenceManager::update(std::vector<Building*>* buildings) const {
	resetMapsF(buildingsInfluencePerPlayer);
	for (auto building : (*buildings)) {
		buildingsInfluencePerPlayer[building->getPlayer()]->update(building);
	}
	calcStats(buildingsInfluencePerPlayer);
}

void InfluenceManager::update(std::vector<ResourceEntity*>* resources) const {}

void InfluenceManager::resetMapsF(const std::vector<InfluenceMapFloat*>& maps) const {
	for (auto map : maps) {
		map->reset();
	}
}

void InfluenceManager::resetMapsI(const std::vector<InfluenceMapInt*>& maps) const {
	for (auto map : maps) {
		map->reset();
	}
}

void InfluenceManager::calcStats(const std::vector<InfluenceMapFloat*>& maps) const {
	for (auto map : maps) {
		map->calcStats();
	}
}

void InfluenceManager::calcStats(const std::vector<InfluenceMapInt*>& maps) const {
	for (auto map : maps) {
		map->calcStats();
	}
}

void InfluenceManager::update(std::vector<Unit*>* units, std::vector<Building*>* buildings) const {
	resetMapsF(attackLevelPerPlayer);
	resetMapsF(defenceLevelPerPlayer);
	resetMapsF(econLevelPerPlayer);

	for (auto unit : (*units)) {
		attackLevelPerPlayer[unit->getPlayer()]->update(unit, unit->getValueOf(ValueType::ATTACK));
		defenceLevelPerPlayer[unit->getPlayer()]->update(unit, unit->getValueOf(ValueType::DEFENCE));
		econLevelPerPlayer[unit->getPlayer()]->update(unit, unit->getValueOf(ValueType::ECON));
	}

	for (auto building : (*buildings)) {
		attackLevelPerPlayer[building->getPlayer()]->update(building, building->getValueOf(ValueType::ATTACK));
		defenceLevelPerPlayer[building->getPlayer()]->update(building, building->getValueOf(ValueType::DEFENCE));
		econLevelPerPlayer[building->getPlayer()]->update(building, building->getValueOf(ValueType::ECON));
	}
	calcStats(attackLevelPerPlayer);
	calcStats(defenceLevelPerPlayer);
	calcStats(econLevelPerPlayer);
}

void InfluenceManager::drawMap(char index, const std::vector<InfluenceMapFloat*>& vector) const {
	index = index % vector.size();
	vector[index]->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
}

void InfluenceManager::draw(InfluenceType type, char index) {
	DebugLineRepo::clear(DebugLineType::INFLUANCE, currentDebugBatch);
	DebugLineRepo::beginGeometry(DebugLineType::INFLUANCE, currentDebugBatch);

	switch (type) {
	case InfluenceType::NONE:
		break;
	case InfluenceType::UNITS_NUMBER_PER_PLAYER:
		index = index % unitsNumberPerPlayer.size();
		unitsNumberPerPlayer[index]->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
		break;
	case InfluenceType::UNITS_INFLUENCE_PER_PLAYER:
		drawMap(index, unitsInfluencePerPlayer);
		break;
	case InfluenceType::BUILDING_INFLUENCE_PER_PLAYER:
		drawMap(index, buildingsInfluencePerPlayer);
		break;
	case InfluenceType::RESOURCE_INFLUENCE:
		drawMap(index, resourceInfluence);
		break;
	case InfluenceType::ATTACK_INFLUENCE_PER_PLAYER:
		drawMap(index, attackLevelPerPlayer);
		break;
	case InfluenceType::DEFENCE_INFLUENCE_PER_PLAYER:
		drawMap(index, defenceLevelPerPlayer);
		break;
	case InfluenceType::ECON_INFLUENCE_PER_PLAYER:
		drawMap(index, econLevelPerPlayer);
		break;


	default: ;
	}

	DebugLineRepo::commit(DebugLineType::INFLUANCE, currentDebugBatch);
	currentDebugBatch++;
	if (currentDebugBatch >= MAX_DEBUG_PARTS_INFLUENCE) {
		currentDebugBatch = 0;
	}
}

void InfluenceManager::switchDebug() {
	switch (debugType) {
	case InfluenceType::NONE:
		debugType = InfluenceType::UNITS_NUMBER_PER_PLAYER;
		break;
	case InfluenceType::UNITS_NUMBER_PER_PLAYER:
		debugType = InfluenceType::UNITS_INFLUENCE_PER_PLAYER;
		break;
	case InfluenceType::UNITS_INFLUENCE_PER_PLAYER:
		debugType = InfluenceType::NONE;
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
				char value = unitsNumberPerPlayer[i]->getValueAt(center);
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

void InfluenceManager::writeInInfluenceDataAt(float* data, char player, const Urho3D::Vector2& pos) {
	data[static_cast<char>(AiInfluenceType::ECON)] = econLevelPerPlayer[player]->getValueAsPercent(pos);
	data[static_cast<char>(AiInfluenceType::ATTACK)] = attackLevelPerPlayer[player]->getValueAsPercent(pos);
	data[static_cast<char>(AiInfluenceType::DEFENCE)] = defenceLevelPerPlayer[player]->getValueAsPercent(pos);

	data[static_cast<char>(AiInfluenceType::BUILDINGS)] = buildingsInfluencePerPlayer[player]->getValueAsPercent(pos);
	data[static_cast<char>(AiInfluenceType::UNITS)] = unitsInfluencePerPlayer[player]->getValueAsPercent(pos);

	data[static_cast<char>(AiInfluenceType::RESOURCE_0)] = resourceInfluence[0]->getValueAsPercent(pos);
	data[static_cast<char>(AiInfluenceType::RESOURCE_1)] = resourceInfluence[1]->getValueAsPercent(pos);
	data[static_cast<char>(AiInfluenceType::RESOURCE_2)] = resourceInfluence[2]->getValueAsPercent(pos);
	data[static_cast<char>(AiInfluenceType::RESOURCE_3)] = resourceInfluence[3]->getValueAsPercent(pos);
}

std::vector<Urho3D::Vector2> InfluenceManager::getAreas(float* result, char player, float tolerance) {
	float econPercent = result[static_cast<char>(AiInfluenceType::ECON)];
	std::vector<int> econIndexes = econLevelPerPlayer[player]->getIndexesWithValueCloseTo(econPercent, tolerance);
}
