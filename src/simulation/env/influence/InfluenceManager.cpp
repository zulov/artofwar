#include "InfluenceManager.h"

#include "map/InfluenceMapHistory.h"
#include "map/InfluenceMapCombine.h"
#include "map/InfluenceMapQuad.h"
#include "debug/DebugLineRepo.h"
#include "map/InfluenceMapInt.h"
#include "math/VectorUtils.h"
#include "objects/CellState.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "simulation/env/ContentInfo.h"
#include "simulation/env/EnvConsts.h"
#include "simulation/env/GridCalculatorProvider.h"
#include "utils/AssertUtils.h"

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
		econQuad.emplace_back(new InfluenceMapQuad(1, 7, InfluenceMapType::HISTORY, BUCKET_GRID_SIZE, 0.5f, 1, 10));
	}

	resourceInfluence = new InfluenceMapFloat(INF_GRID_SIZE, BUCKET_GRID_SIZE, 0.5f, INF_LEVEL, 40);

	for (char player = 0; player < numberOfPlayers; ++player) {
		mapsForAiPerPlayer.emplace_back(std::array<InfluenceMapFloat*, 5>{
			buildingsInfluencePerPlayer[player],
			unitsInfluencePerPlayer[player],
			resourceInfluence,
			attackSpeed[player],
			gatherSpeed[player]
		}); //TODO more?
		mapsForCentersPerPlayer.emplace_back(std::array<InfluenceMapQuad*, 3>{
			econQuad[player],
			buildingsQuad[player],
			unitsQuad[player]
		});
		assert(validSizes(mapsForAiPerPlayer.at(player)));
	}

	calculator = GridCalculatorProvider::get(INF_GRID_SIZE, BUCKET_GRID_SIZE);
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
	clear_vector(econQuad);
	delete ci;
}

void InfluenceManager::update(std::vector<Unit*>* units) const {
	resetMaps(unitsNumberPerPlayer);
	resetMaps(unitsInfluencePerPlayer);

	for (auto unit : (*units)) {
		auto pId = unit->getPlayer();
		unitsNumberPerPlayer[pId]->updateInt(unit);

		unitsInfluencePerPlayer[pId]->tempUpdate(unit);
	}

	calcStats(unitsNumberPerPlayer);
	calcStats(unitsInfluencePerPlayer);
}

void InfluenceManager::update(std::vector<Building*>* buildings) const {
	resetMaps(buildingsInfluencePerPlayer);
	for (auto building : (*buildings)) {
		buildingsInfluencePerPlayer[building->getPlayer()]->tempUpdate(building);
	}
	calcStats(buildingsInfluencePerPlayer);
}

void InfluenceManager::update(std::vector<ResourceEntity*>* resources) const {
	resourceInfluence->reset();
	for (auto resource : (*resources)) {
		resourceInfluence->tempUpdate(resource, resource->getHealthPercent());
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
	resetMaps(econQuad);

	for (auto unit : (*units)) {
		unitsQuad[unit->getPlayer()]->updateInt(unit);
	}
	for (auto building : (*buildings)) {
		buildingsQuad[building->getPlayer()]->updateInt(building);
	}
	calcStats(unitsQuad);
	calcStats(buildingsQuad);
	calcStats(econQuad);
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
	vector[index]->get(cast(type))->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
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
	drawAll(econQuad, "econQuad");
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
	case CellState::NONE:
	case CellState::ATTACK:
	case CellState::COLLECT:
	case CellState::DEPLOY:
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

std::array<float, 5>& InfluenceManager::getInfluenceDataAt(char player, const Urho3D::Vector2& pos) {
	auto& array = mapsForAiPerPlayer[player];
	assert(array.size()==dataFromPos.size());
	for (int i = 0; i < array.size(); ++i) {
		dataFromPos[i] = array[i]->getValueAsPercent(pos);
	}
	return dataFromPos;
}

std::vector<int> InfluenceManager::getIndexesIterative(const std::span<float> result, float tolerance, int min,
                                                       std::array<InfluenceMapFloat*, 5>& maps) const {
	int k = 0;
	for (auto step : {0.0f, 0.05f, 0.1f}) {
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
                                                                 float tolerance, int min) {
	auto& maps = mapsForAiPerPlayer[player];
	assert(result.size()==maps.size());

	return centersFromIndexes(getIndexesIterative(result, tolerance, min, maps));
}

std::vector<Urho3D::Vector2> InfluenceManager::getAreas(const std::span<float> result, char player) {
	auto& maps = mapsForAiPerPlayer[player];
	assert(result.size()==maps.size());

	std::fill_n(intersection, arraySize, 0.f);

	for (char i = 0; i < maps.size(); ++i) {
		maps[i]->getIndexesWithByValue(result[i], intersection);
	}

	auto inx = sort_indexes(intersection, arraySize);
	return centersFromIndexes(intersection, inx, 0.02f * maps.size());
}

void InfluenceManager::addCollect(Unit* unit, float value) {
	gatherSpeed[unit->getPlayer()]->update(unit, value);
	econQuad[unit->getPlayer()]->update(unit, value);
}

void InfluenceManager::addAttack(Unit* unit, float value) {
	attackSpeed[unit->getPlayer()]->update(unit, value);
}

Urho3D::Vector2 InfluenceManager::getCenterOf(CenterType id, char player) {
	 return mapsForCentersPerPlayer[player][cast(id)]->getCenter();
}

std::vector<Urho3D::Vector2> InfluenceManager::centersFromIndexes(float* values,
                                                                  const std::vector<unsigned>& indexes,
                                                                  float minVal) const {
	std::vector<Urho3D::Vector2> centers;

	for (auto ptr = indexes.begin(); (ptr < indexes.begin() + 256 && ptr < indexes.end()); ++ptr) {
		auto value = values[*ptr];
		if (value > minVal) {
			break;
		}
		centers.emplace_back(calculator->getCenter(*ptr));
	}
	return centers;
}

std::vector<Urho3D::Vector2> InfluenceManager::centersFromIndexes(const std::vector<int>& intersection) const {
	std::vector<Urho3D::Vector2> centers;
	centers.reserve(intersection.size());
	for (auto value : intersection) {
		centers.emplace_back(calculator->getCenter(value));
	}
	return centers;
}

float InfluenceManager::getFieldSize() const {
	return resourceInfluence->getFieldSize();
}
