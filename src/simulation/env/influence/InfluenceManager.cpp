#include "InfluenceManager.h"

#include <chrono>
#include <span>
#include "Game.h"
#include "InfluenceMapType.h"
#include "database/DatabaseCache.h"
#include "debug/DebugLineRepo.h"
#include "math/VectorUtils.h"
#include "objects/CellState.h"
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

		gatherSpeed.emplace_back(
			new InfluenceMapHistory(INF_GRID_SIZE, BUCKET_GRID_SIZE, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40));

		attackSpeed.emplace_back(
			new InfluenceMapHistory(INF_GRID_SIZE, BUCKET_GRID_SIZE, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40));
		unitsQuad.emplace_back(new InfluenceMapQuad(1, 7, InfluenceMapType::INT, BUCKET_GRID_SIZE, 0.5f, 1, 2));
		buildingsQuad.emplace_back(new InfluenceMapQuad(1, 7, InfluenceMapType::INT, BUCKET_GRID_SIZE, 0.5f, 1, 2));
		gatherQuad.emplace_back(new InfluenceMapQuad(1, 7, InfluenceMapType::HISTORY, BUCKET_GRID_SIZE, 0.5f, 1, 10));
	}

	resourceInfluence=new InfluenceMapFloat(INF_GRID_SIZE, BUCKET_GRID_SIZE, 0.5f, INF_LEVEL, 40);
	
	for (char player = 0; player < numberOfPlayers; ++player) {
		mapsForAiPerPlayer.emplace_back(std::array<InfluenceMapFloat*, 6>{
			buildingsInfluencePerPlayer[player],
			unitsInfluencePerPlayer[player],
			resourceInfluence
		});
	}


	ci = new content_info();
	DebugLineRepo::init(DebugLineType::INFLUENCE, MAX_DEBUG_PARTS_INFLUENCE);
}

InfluenceManager::~InfluenceManager() {
	clear_vector(unitsNumberPerPlayer);
	clear_vector(buildingsInfluencePerPlayer);
	clear_vector(unitsInfluencePerPlayer);

	clear_vector(gatherSpeed);
	clear_vector(attackSpeed);
	delete resourceInfluence;

	clear_vector(unitsQuad);
	clear_vector(buildingsQuad);
	clear_vector(gatherQuad);
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
	resourceInfluence->reset();
	for (auto resource : (*resources)) {
		resourceInfluence->update(resource, resource->getHealthPercent());
	}
	resourceInfluence->finishCalc();
}

template <typename T>
void InfluenceManager::resetMaps(const std::vector<T*>& maps) const {
	for (auto map : maps) {
		map->reset();
	}
}

template <typename T>
void InfluenceManager::calcStats(const std::vector<T*>& maps) const {
	for (auto map : maps) {
		map->finishCalc();
	}
}

template <typename T>
void InfluenceManager::drawAll(const std::vector<T*>& maps, Urho3D::String name) const {
	for (int i = 0; i < maps.size(); ++i) {
		maps[i]->print(name + "_" + Urho3D::String(i) + "_");
	}
}

void InfluenceManager::updateQuad(std::vector<Unit*>* units, std::vector<Building*>* buildings) const {
	resetMaps(unitsQuad);
	resetMaps(buildingsQuad);
	resetMaps(gatherQuad);

	for (auto unit : (*units)) {
		unitsQuad[unit->getPlayer()]->update(unit);
	}
	for (auto building : (*buildings)) {
		buildingsQuad[building->getPlayer()]->update(building);
	}
	calcStats(unitsQuad);
	calcStats(buildingsQuad);
	calcStats(gatherQuad);
}

void InfluenceManager::updateWithHistory() const {
	calcStats(gatherSpeed);
	calcStats(attackSpeed);

	resetMaps(gatherSpeed);
	resetMaps(attackSpeed);
}

void InfluenceManager::drawMap(char index, const std::vector<InfluenceMapFloat*>& vector) const {
	index = index % vector.size();
	vector[index]->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
}

void InfluenceManager::drawMap(char index, const std::vector<InfluenceMapCombine*>& vector, char type) const {
	index = index % vector.size();
	vector[index]->get(static_cast<char>(type))->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
}

void InfluenceManager::draw(InfluenceDataType type, char index) {
	DebugLineRepo::clear(DebugLineType::INFLUENCE, currentDebugBatch);
	DebugLineRepo::beginGeometry(DebugLineType::INFLUENCE, currentDebugBatch);

	switch (type) {
	case InfluenceDataType::NONE:
		break;
	case InfluenceDataType::UNITS_NUMBER_PER_PLAYER:
		index = index % unitsNumberPerPlayer.size();
		unitsNumberPerPlayer[index]->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
		break;
	case InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER:
		drawMap(index, unitsInfluencePerPlayer);
		break;
	case InfluenceDataType::BUILDING_INFLUENCE_PER_PLAYER:
		drawMap(index, buildingsInfluencePerPlayer);
		break;
	case InfluenceDataType::RESOURCE_INFLUENCE:
		resourceInfluence->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
		break;
	default: ;
	}

	DebugLineRepo::commit(DebugLineType::INFLUENCE, currentDebugBatch);
	currentDebugBatch++;
	if (currentDebugBatch >= MAX_DEBUG_PARTS_INFLUENCE) {
		currentDebugBatch = 0;
	}
}

void InfluenceManager::drawAll() {
	drawAll(unitsNumberPerPlayer, "unitsInt");
	drawAll(buildingsInfluencePerPlayer, "buildingsInt");
	drawAll(unitsInfluencePerPlayer, "units");
	resourceInfluence->print("resource_");

	drawAll(gatherSpeed, "gather");
	drawAll(attackSpeed, "attack");

	drawAll(unitsQuad, "unitsQuad");
	drawAll(buildingsQuad, "buildingsQuad");
	drawAll(gatherQuad, "gatherQuad");
}

void InfluenceManager::switchDebug() {
	switch (debugType) {
	case InfluenceDataType::NONE:
		debugType = InfluenceDataType::UNITS_NUMBER_PER_PLAYER;
		break;
	case InfluenceDataType::UNITS_NUMBER_PER_PLAYER:
		debugType = InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER;
		break;
	case InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER:
		debugType = InfluenceDataType::NONE;
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

std::array<float, 6>& InfluenceManager::getInfluenceDataAt(char player, const Urho3D::Vector2& pos) {
	auto& array = mapsForAiPerPlayer[player];
	assert(array.size()==dataFromPos.size());
	for (int i = 0; i < array.size(); ++i) {
		dataFromPos[i] = array[i]->getValueAsPercent(pos);
	}
	return dataFromPos;
}

std::vector<int> InfluenceManager::getIndexesIterative(const std::span<float> result, float tolerance, int min,
                                                       std::array<InfluenceMapFloat*, 6>& maps) const {
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
	auto constexpr arraySize = INF_GRID_SIZE * INF_GRID_SIZE;
	float intersection[arraySize];
	std::fill_n(intersection, arraySize, 0.f);

	for (char i = 0; i < maps.size(); ++i) {
		maps[i]->getIndexesWithByValue(result[i], intersection);
	}
	auto inx = sort_indexes(intersection, arraySize);
	return centersFromIndexes(maps[0], intersection, inx, 0.02f * maps.size());
}

void InfluenceManager::addCollect(Unit* unit, float value) {
	gatherSpeed[unit->getPlayer()]->update(unit, value);
	gatherQuad[unit->getPlayer()]->update(unit, value);
}

void InfluenceManager::addAttack(Unit* unit, float value) {
	attackSpeed[unit->getPlayer()]->update(unit, value);
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

float InfluenceManager::getFieldSize() const {
	return resourceInfluence->getFieldSize();
}
