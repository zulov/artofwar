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
#include "objects/resource/ResourceEntity.h"


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
	for (int player = 0; player < numberOfPlayers; ++player) {
		mapsForAiPerPlayer.emplace_back(std::vector<InfluenceMapFloat*>{
			econLevelPerPlayer[player], attackLevelPerPlayer[player], defenceLevelPerPlayer[player],
			buildingsInfluencePerPlayer[player], unitsInfluencePerPlayer[player],
			resourceInfluence[0], resourceInfluence[1], resourceInfluence[2], resourceInfluence[3]
		}); //TODO performance posortowac tak żeby pierwsze zwracły mniej
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

void InfluenceManager::update(std::vector<ResourceEntity*>* resources) const {
	resetMapsF(resourceInfluence);
	for (auto resource : (*resources)) {
		resourceInfluence[resource->getId()]->update(resource, resource->getHealthPercent());
	}
	calcStats(resourceInfluence);
}

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

std::vector<float>& InfluenceManager::getInfluenceDataAt(char player, const Urho3D::Vector2& pos) {
	dataFromPos.clear();
	auto& maps = mapsForAiPerPlayer[player];
	for (auto map : maps) {
		dataFromPos.push_back(map->getValueAsPercent(pos));
	}
	return dataFromPos;
}

std::vector<int> InfluenceManager::getIndexesIterative(const std::vector<float>& result, float tolerance, int min,
                                                       std::vector<InfluenceMapFloat*>& maps) const {
	float steps[] = {0.0, 0.05, 0.1};
	int k = 0;
	for (auto step : steps) {
		tolerance += step;
		k++;
		std::vector<int> intersection = maps[0]->getIndexesWithByValue(result[0], tolerance);
		for (char i = 1; i < maps.size(); ++i) {
			std::vector<int> indexes = maps[i]->getIndexesWithByValue(result[i], tolerance);
			std::vector<int> temp;
			std::set_intersection(intersection.begin(), intersection.end(),
			                      indexes.begin(), indexes.end(),
			                      std::back_inserter(temp));
			if (temp.empty() || temp.size() < min && k != 3) {
				intersection.clear();
				break;
			}
			intersection = temp; //TODO optimize, nie kopiować?
		}
		if (!intersection.empty()) {
			return intersection;
		}
	}
	return {};
}

std::vector<int> InfluenceManager::getIndexes(const std::vector<float>& result, float tolerance,
                                              std::vector<InfluenceMapFloat*>& maps) const {
	std::vector<int> intersection = maps[0]->getIndexesWithByValue(result[0], tolerance);
	std::cout << intersection.size() << "|";
	for (char i = 1; i < maps.size(); ++i) {
		std::vector<int> indexes = maps[i]->getIndexesWithByValue(result[i], tolerance);
		//TODO performance pass indexex end check only them
		std::cout << indexes.size() << "|";
		std::vector<int> temp;
		std::set_intersection(intersection.begin(), intersection.end(),
		                      indexes.begin(), indexes.end(),
		                      std::back_inserter(temp));
		if (temp.empty()) {
			std::cout << std::endl;
			return {};
		}
		intersection = temp; //TODO optimize, nie kopiować?
	}
	std::cout << std::endl;
	return intersection;
}

std::vector<Urho3D::Vector2> InfluenceManager::getAreasIterative(const std::vector<float>& result, char player,
                                                                 float tolerance,
                                                                 int min) {
	auto& maps = mapsForAiPerPlayer[player];

	std::vector<int> intersection = getIndexesIterative(result, tolerance, min, maps);
	return centersFromIndexes(maps[0], intersection);
}


std::vector<Urho3D::Vector2>
InfluenceManager::getAreas(const std::vector<float>& result, char player, float tolerance) {
	auto& maps = mapsForAiPerPlayer[player];

	std::vector<int> intersection = getIndexes(result, tolerance, maps);
	return centersFromIndexes(maps[0], intersection);
}

void InfluenceManager::getAreas1(const std::vector<float>& result, char player, std::vector<float> tolerances) {
	auto& maps = mapsForAiPerPlayer[player];
	unsigned char intersection[DEFAULT_INF_FLOAT_GRID_SIZE * DEFAULT_INF_FLOAT_GRID_SIZE];
	for (auto tolerance : tolerances) {
		for (char i = 0; i < maps.size(); ++i) {
			maps[i]->getIndexesWithByValu1e(result[i], tolerance, intersection);//TODO PERFORMANCE nie trzeba zwracac tylko zwiekszac w srodku
		}
	}
	//return intersection;
}

std::vector<Urho3D::Vector2> InfluenceManager::centersFromIndexes(InfluenceMapFloat* map,
                                                                  const std::vector<int>& intersection) {
	std::vector<Urho3D::Vector2> centers;
	centers.reserve(intersection.size());
	for (auto value : intersection) {
		centers.emplace_back(map->getCenter(value));
	}
	return centers;
}

float InfluenceManager::getFieldSize() {
	return resourceInfluence[0]->getFieldSize();
}
