#include "InfluenceManager.h"

#include "map/InfluenceMapHistory.h"
#include "map/InfluenceMapQuad.h"
#include "debug/DebugLineRepo.h"
#include "map/InfluenceMapInt.h"
#include "math/VectorUtils.h"
#include "objects/CellState.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "simulation/env/ContentInfo.h"
#include "simulation/env/GridCalculatorProvider.h"
#include "utils/AssertUtils.h"

constexpr char MAX_DEBUG_PARTS_INFLUENCE = 32;
constexpr char INF_LEVEL = 4;
constexpr short INF_GRID_FIELD_SIZE = 8.f;


InfluenceManager::InfluenceManager(char numberOfPlayers, float mapSize) {
	for (int i = 0; i < numberOfPlayers; ++i) {
		unitsNumberPerPlayer.emplace_back(new InfluenceMapInt(mapSize / INF_GRID_FIELD_SIZE, mapSize, 40));
		buildingsInfluencePerPlayer.emplace_back(
			new InfluenceMapFloat(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 2));
		unitsInfluencePerPlayer.emplace_back(
			new InfluenceMapFloat(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 40));

		gatherSpeed.emplace_back(
			new InfluenceMapHistory(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40));

		attackSpeed.emplace_back(
			new InfluenceMapHistory(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40));
		unitsQuad.emplace_back(new InfluenceMapQuad(mapSize / INF_GRID_FIELD_SIZE, mapSize));
		buildingsQuad.emplace_back(new InfluenceMapQuad(mapSize / INF_GRID_FIELD_SIZE, mapSize));
		econQuad.emplace_back(new InfluenceMapQuad(mapSize / INF_GRID_FIELD_SIZE, mapSize));
	}

	resourceInfluence = new InfluenceMapFloat(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 40);

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

	calculator = GridCalculatorProvider::get(mapSize / INF_GRID_FIELD_SIZE, mapSize);
	ci = new content_info();
	DebugLineRepo::init(DebugLineType::INFLUENCE, MAX_DEBUG_PARTS_INFLUENCE);

	assert(unitsNumberPerPlayer[0]->getResolution() == unitsInfluencePerPlayer[0]->getResolution()
		&& calculator->getResolution() == unitsNumberPerPlayer[0]->getResolution());
	arraySize = calculator->getResolution() * calculator->getResolution();
	intersection = new float[arraySize];
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

	delete[]intersection;
}

void InfluenceManager::update(std::vector<Unit*>* units) const {
	resetMaps(unitsNumberPerPlayer);
	resetMaps(unitsInfluencePerPlayer);

	for (auto unit : (*units)) {
		const auto pId = unit->getPlayer();
		const auto index = calculator->indexFromPosition(unit->getPosition());
		unitsNumberPerPlayer[pId]->updateInt(index);
		unitsInfluencePerPlayer[pId]->tempUpdate(index);
	}
	finalize(unitsInfluencePerPlayer);
}

void InfluenceManager::update(std::vector<Building*>* buildings) const {
	resetMaps(buildingsInfluencePerPlayer);
	for (auto building : (*buildings)) {
		buildingsInfluencePerPlayer[building->getPlayer()]->tempUpdate(building);
	}
	finalize(buildingsInfluencePerPlayer);
}

void InfluenceManager::update(std::vector<ResourceEntity*>* resources) const {
	resourceInfluence->reset();
	for (auto resource : (*resources)) {
		resourceInfluence->tempUpdate(resource, resource->getHealthPercent());
	}
	resourceInfluence->updateFromTemp();
}

void InfluenceManager::updateQuadUnits(std::vector<Unit*>* units) const {
	resetMaps(unitsQuad);
	for (auto unit : (*units)) {
		unitsQuad[unit->getPlayer()]->updateInt(unit);
	}
}

void InfluenceManager::updateQuadBuildings(std::vector<Building*>* buildings) const {
	resetMaps(buildingsQuad);
	for (auto building : (*buildings)) {
		buildingsQuad[building->getPlayer()]->updateInt(building);
	}
}

void InfluenceManager::updateWithHistory() const {
	resetMaps(gatherSpeed);
	resetMaps(attackSpeed);

	finalize(gatherSpeed);
	finalize(attackSpeed);
}

void InfluenceManager::updateQuadOther() const {
	resetMaps(econQuad);
}

template <typename T>
void InfluenceManager::resetMaps(const std::vector<T*>& maps) const {
	for (auto map : maps) {
		map->reset();
	}
}

template <typename T>
void InfluenceManager::finalize(const std::vector<T*>& maps) const {
	for (auto map : maps) {
		map->updateFromTemp();
	}
}

template <typename T>
void InfluenceManager::drawAll(const std::vector<T*>& maps, Urho3D::String name) const {
	for (int i = 0; i < maps.size(); ++i) {
		maps[i]->print(name + "_" + Urho3D::String(i) + "_");
	}
}


void InfluenceManager::drawMap(char index, const std::vector<InfluenceMapFloat*>& vector) const {
	index = index % vector.size();
	vector[index]->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
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

void InfluenceManager::drawAll() const {
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
	ci->reset(); //TODO przemyslec to, zbyt skomplikowane
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
		array[i]->computeMinMax();
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

std::vector<int> InfluenceManager::getAreas(const std::span<float> result, char player) {
	auto& maps = mapsForAiPerPlayer[player];
	assert(result.size()==maps.size());

	std::fill_n(intersection, arraySize, 0.f);

	for (char i = 0; i < maps.size(); ++i) {
		maps[i]->getIndexesWithByValue(result[i], intersection);
	}
	//TODO pref std::partial_sort
	const auto inx = sort_indexes(std::span(intersection, arraySize), 256);
	return centersFromIndexes(intersection, inx, 0.02f * maps.size());
}

void InfluenceManager::addCollect(Unit* unit, float value) {
	const auto playerId = unit->getPlayer();
	assert(gatherSpeed[playerId]->getResolution()==econQuad[playerId]->getResolution());
	assert(gatherSpeed[playerId]->getResolution()==calculator->getResolution());

	auto index = calculator->indexFromPosition(unit->getPosition());

	gatherSpeed[playerId]->tempUpdate(index, value);
	econQuad[playerId]->update(index, value);
}

void InfluenceManager::addAttack(Unit* unit, float value) {
	attackSpeed[unit->getPlayer()]->tempUpdate(unit, value);
}

std::optional<Urho3D::Vector2> InfluenceManager::getCenterOf(CenterType id, char player) {
	return mapsForCentersPerPlayer[player][cast(id)]->getCenter();
}

std::vector<int> InfluenceManager::centersFromIndexes(float* values, const std::vector<unsigned>& indexes,
                                                      float minVal) const {
	std::vector<int> result;

	for (auto ptr = indexes.begin(); (ptr < indexes.begin() + 256 && ptr < indexes.end()); ++ptr) {
		if (values[*ptr] > minVal) {
			break;
		}
		result.emplace_back(*ptr);
	}
	return result;
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
