#include "InfluenceManager.h"

#include <chrono>
#include <span>
#include "Game.h"
#include "database/DatabaseCache.h"
#include "debug/DebugLineRepo.h"
#include "math/VectorUtils.h"
#include "objects/CellState.h"
#include "objects/ValueType.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "simulation/env/ContentInfo.h"
#include "simulation/env/EnvConsts.h"

constexpr short INF_GRID_SIZE = 128;
constexpr char MAX_DEBUG_PARTS_INFLUENCE = 32;
constexpr char INF_LEVEL = 4;


InfluenceManager::InfluenceManager(char numberOfPlayers) {
	for (int i = 0; i < numberOfPlayers; ++i) {
		unitsNumberPerPlayer.emplace_back(new InfluenceMapInt(INF_GRID_SIZE, BUCKET_GRID_SIZE, 40));
		buildingsInfluencePerPlayer.emplace_back(
			new InfluenceMapFloat(INF_GRID_SIZE, BUCKET_GRID_SIZE, 0.5f, INF_LEVEL, 2));
		unitsInfluencePerPlayer.emplace_back(
			new InfluenceMapFloat(INF_GRID_SIZE, BUCKET_GRID_SIZE, 0.5f, INF_LEVEL, 40));

		basicValues.emplace_back(
			new InfluenceMapCombine(INF_GRID_SIZE, BUCKET_GRID_SIZE, 0.5f, INF_LEVEL, 40,
			                        magic_enum::enum_count<ValueType>()));
	}

	for (int i = 0; i < Game::getDatabase()->getResourceSize(); ++i) {
		resourceInfluence.
			emplace_back(new InfluenceMapFloat(INF_GRID_SIZE, BUCKET_GRID_SIZE, 0.5f, INF_LEVEL, 40));
	}
	for (char player = 0; player < numberOfPlayers; ++player) {
		mapsForAiPerPlayer.emplace_back(std::vector<InfluenceMapFloat*>{
			basicValues[player]->get(static_cast<char>(ValueType::ECON)),
			basicValues[player]->get(static_cast<char>(ValueType::ATTACK)),
			basicValues[player]->get(static_cast<char>(ValueType::DEFENCE)),
			buildingsInfluencePerPlayer[player], unitsInfluencePerPlayer[player],
			resourceInfluence[0], resourceInfluence[1], resourceInfluence[2],
			resourceInfluence[3] //TODO moze to nie osobno jednak? za duza ma wage
		});
	}

	main = new InfluenceMapCombine(INF_GRID_SIZE, BUCKET_GRID_SIZE, 0.5, 16, 40, numberOfPlayers);
	ci = new content_info();
	DebugLineRepo::init(DebugLineType::INFLUANCE, MAX_DEBUG_PARTS_INFLUENCE);
}

InfluenceManager::~InfluenceManager() {
	clear_vector(unitsNumberPerPlayer);
	clear_vector(buildingsInfluencePerPlayer);
	clear_vector(unitsInfluencePerPlayer);

	delete main;
	delete ci;
}

void InfluenceManager::update(std::vector<Unit*>* units) const {
	resetMaps(unitsNumberPerPlayer);
	resetMaps(unitsInfluencePerPlayer);

	for (auto unit : (*units)) {
		unitsNumberPerPlayer[unit->getPlayer()]->update(unit);
		unitsInfluencePerPlayer[unit->getPlayer()]->update(unit);
	}
	calcStats(unitsNumberPerPlayer);
	calcStats(unitsInfluencePerPlayer);
}

void InfluenceManager::update(std::vector<Building*>* buildings) const {
	resetMaps(buildingsInfluencePerPlayer);
	for (auto building : (*buildings)) {
		buildingsInfluencePerPlayer[building->getPlayer()]->update(building);
	}
	calcStats(buildingsInfluencePerPlayer);
}

void InfluenceManager::update(std::vector<ResourceEntity*>* resources) const {
	resetMaps(resourceInfluence);
	for (auto resource : (*resources)) {
		resourceInfluence[resource->getId()]->update(resource, resource->getHealthPercent());
	}
	calcStats(resourceInfluence);
}

void InfluenceManager::resetMaps(const std::vector<InfluenceMapFloat*>& maps) const {
	std::for_each(maps.begin(), maps.end(), resetMap);
}

void InfluenceManager::resetMaps(const std::vector<InfluenceMapInt*>& maps) const {
	std::for_each(maps.begin(), maps.end(), resetMap);
}

void InfluenceManager::resetMaps(const std::vector<InfluenceMapCombine*>& maps) const {
	std::for_each(maps.begin(), maps.end(), resetMap);
}

void InfluenceManager::calcStats(const std::vector<InfluenceMapFloat*>& maps) const {
	std::for_each(maps.begin(), maps.end(), finishCalc);
}

void InfluenceManager::calcStats(const std::vector<InfluenceMapInt*>& maps) const {
	std::for_each(maps.begin(), maps.end(), finishCalc);
}

void InfluenceManager::calcStats(const std::vector<InfluenceMapCombine*>& maps) const {
	std::for_each(maps.begin(), maps.end(), finishCalc);
}

void InfluenceManager::basicValuesFunc(float* weights, Physical* thing) const {
	auto data = std::span{weights, 3};
	thing->fillValues(data);

	basicValues[thing->getPlayer()]->update(thing, data);
}

void InfluenceManager::updateBasic(std::vector<Unit*>* units, std::vector<Building*>* buildings) const {
	resetMaps(basicValues);
	float weights[3] = {-1}; //TODO hardcode
	for (auto unit : (*units)) {
		basicValuesFunc(weights, unit);
	}

	for (auto building : (*buildings)) {
		basicValuesFunc(weights, building);
	}
	calcStats(basicValues);
}

void InfluenceManager::updateMain(std::vector<Unit*>* units, std::vector<Building*>* buildings) const {
	main->reset();

	float weights[MAX_PLAYERS] = {-1}; //TODO hardcode
	for (auto unit : (*units)) {
		std::fill_n(weights, MAX_PLAYERS, -1.f);
		weights[unit->getPlayer()] = 1.f;
		main->update(unit, std::span{weights, MAX_PLAYERS});
	}

	for (auto building : (*buildings)) {
		std::fill_n(weights, MAX_PLAYERS, -1.f);
		weights[building->getPlayer()] = 1.f;
		main->update(building, std::span{weights, MAX_PLAYERS});
	}

	main->finishCalc();
}

void InfluenceManager::drawMap(char index, const std::vector<InfluenceMapFloat*>& vector) const {
	index = index % vector.size();
	vector[index]->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
}

void InfluenceManager::drawMap(char index, const std::vector<InfluenceMapCombine*>& vector, ValueType type) const {
	index = index % vector.size();
	vector[index]->get(static_cast<char>(type))->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
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
	case InfluenceType::ECON_INFLUENCE_PER_PLAYER:
		drawMap(index, basicValues, ValueType::ECON);
		break;
	case InfluenceType::ATTACK_INFLUENCE_PER_PLAYER:
		drawMap(index, basicValues, ValueType::ATTACK);
		break;
	case InfluenceType::DEFENCE_INFLUENCE_PER_PLAYER:
		drawMap(index, basicValues, ValueType::DEFENCE);
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

std::vector<int> InfluenceManager::getIndexesIterative(const std::span<float> result, float tolerance, int min,
                                                       std::vector<InfluenceMapFloat*>& maps) const {
	int k = 0;
	for (auto step : {0.0, 0.05, 0.1}) {
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

std::vector<Urho3D::Vector2> InfluenceManager::getAreasIterative(const std::span<float> result, char player,
                                                                 float tolerance,
                                                                 int min) {
	auto& maps = mapsForAiPerPlayer[player];
	assert(result.size()==maps.size());

	return centersFromIndexes(maps[0], getIndexesIterative(result, tolerance, min, maps));
}

std::vector<Urho3D::Vector2> InfluenceManager::getAreas(const std::span<float> result, char player) {
	auto& maps = mapsForAiPerPlayer[player];
	assert(result.size()==maps.size());
	auto arraySize = INF_GRID_SIZE * INF_GRID_SIZE;
	float intersection[INF_GRID_SIZE * INF_GRID_SIZE];
	std::fill_n(intersection, arraySize, 0.f);

	for (char i = 0; i < maps.size(); ++i) {
		maps[i]->getIndexesWithByValue(result[i], intersection);
	}
	auto inx = sort_indexes(intersection, arraySize);
	return centersFromIndexes(maps[0], intersection, inx, 0.02f * maps.size());
}

std::vector<Urho3D::Vector2> InfluenceManager::centersFromIndexes(InfluenceMapFloat* map, float* values,
                                                                  const std::vector<unsigned>& indexes,
                                                                  float minVal) const {
	std::vector<Urho3D::Vector2> centers;

	for (auto ptr = indexes.begin(); (ptr < indexes.begin() + 256 && ptr < indexes.end()); ++ptr) {
		auto value = values[*ptr];
		if (value > minVal) {
			break;
		}
		centers.emplace_back(map->getCenter(*ptr));
	}
	return centers;
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
