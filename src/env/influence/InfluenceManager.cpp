#include "InfluenceManager.h"

#include <exprtk/exprtk.hpp>

#include "MapsUtils.h"
#include "VisibilityManager.h"
#include "map/InfluenceMapHistory.h"
#include "map/InfluenceMapQuad.h"
#include "debug/DebugLineRepo.h"
#include "map/InfluenceMapInt.h"
#include "math/VectorUtils.h"
#include "objects/CellState.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "env/ContentInfo.h"
#include "env/GridCalculatorProvider.h"
#include "utils/AssertUtils.h"
#include "utils/OtherUtils.h"


InfluenceManager::InfluenceManager(char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain) {
	unitsNumberPerPlayer.reserve(numberOfPlayers);
	buildingsInfluencePerPlayer.reserve(numberOfPlayers);
	unitsInfluencePerPlayer.reserve(numberOfPlayers);
	foodGatherSpeed.reserve(numberOfPlayers);
	woodGatherSpeed.reserve(numberOfPlayers);
	stoneGatherSpeed.reserve(numberOfPlayers);
	goldGatherSpeed.reserve(numberOfPlayers);
	attackSpeed.reserve(numberOfPlayers);
	armyQuad.reserve(numberOfPlayers);
	buildingsQuad.reserve(numberOfPlayers);
	econQuad.reserve(numberOfPlayers);

	mapsForAiArmyPerPlayer.reserve(numberOfPlayers);
	mapsForAiPerPlayer.reserve(numberOfPlayers);
	mapsForCentersPerPlayer.reserve(numberOfPlayers);

	for (int player = 0; player < numberOfPlayers; ++player) {
		unitsNumberPerPlayer.emplace_back(new InfluenceMapInt(mapSize / INF_GRID_FIELD_SIZE, mapSize, 40));
		buildingsInfluencePerPlayer.emplace_back(
			new InfluenceMapFloat(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 2));
		unitsInfluencePerPlayer.emplace_back(
			new InfluenceMapFloat(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 40));

		foodGatherSpeed.emplace_back(
			new InfluenceMapHistory(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40));
		woodGatherSpeed.emplace_back(
			new InfluenceMapHistory(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40));
		stoneGatherSpeed.emplace_back(
			new InfluenceMapHistory(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40));
		goldGatherSpeed.emplace_back(
			new InfluenceMapHistory(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40));

		attackSpeed.emplace_back(
			new InfluenceMapHistory(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40));
		armyQuad.emplace_back(new InfluenceMapQuad(mapSize / INF_GRID_FIELD_SIZE, mapSize));
		buildingsQuad.emplace_back(new InfluenceMapQuad(mapSize / INF_GRID_FIELD_SIZE, mapSize));
		econQuad.emplace_back(new InfluenceMapQuad(mapSize / INF_GRID_FIELD_SIZE, mapSize));
	}

	resourceInfluence = new InfluenceMapFloat(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 40);
	for (int player = 0; player < numberOfPlayers; ++player) {
		mapsForAiPerPlayer.emplace_back(std::array<InfluenceMapFloat*, 8>{
			buildingsInfluencePerPlayer[player],
			unitsInfluencePerPlayer[player],
			resourceInfluence, //TODO czy to jest ważne?
			attackSpeed[player],
			foodGatherSpeed[player],
			woodGatherSpeed[player],
			stoneGatherSpeed[player],
			goldGatherSpeed[player],
		});
		mapsForAiArmyPerPlayer.emplace_back(std::array<InfluenceMapFloat*, 3>{
			buildingsInfluencePerPlayer[player],
			unitsInfluencePerPlayer[player],
			attackSpeed[player],
		});
		mapsForCentersPerPlayer.emplace_back(std::array<InfluenceMapQuad*, 3>{
			econQuad[player],
			buildingsQuad[player],
			armyQuad[player]
		});
		assert(validSizes(mapsForAiPerPlayer.at(player)));
	}
	visibilityManager = new VisibilityManager(numberOfPlayers, mapSize, terrain);

	calculator = GridCalculatorProvider::get(mapSize / INF_GRID_FIELD_SIZE, mapSize);
	ci = new content_info();
	DebugLineRepo::init(DebugLineType::INFLUENCE, MAX_DEBUG_PARTS_INFLUENCE);

	arraySize = calculator->getResolution() * calculator->getResolution();
	assert(arraySize <= std::numeric_limits<unsigned short>::max());
	intersection = new float[arraySize];
	tempIndexes = new std::vector<int>();

	assert(unitsNumberPerPlayer[0]->getResolution() == unitsInfluencePerPlayer[0]->getResolution()
		&& calculator->getResolution() == unitsNumberPerPlayer[0]->getResolution());
}

InfluenceManager::~InfluenceManager() {
	clear_vector(unitsNumberPerPlayer);
	clear_vector(buildingsInfluencePerPlayer);
	clear_vector(unitsInfluencePerPlayer);

	clear_vector(foodGatherSpeed);
	clear_vector(woodGatherSpeed);
	clear_vector(stoneGatherSpeed);
	clear_vector(goldGatherSpeed);

	clear_vector(attackSpeed);
	delete resourceInfluence;

	clear_vector(armyQuad);
	clear_vector(buildingsQuad);
	clear_vector(econQuad);

	delete visibilityManager;
	delete ci;

	delete[]intersection;
	delete tempIndexes;
}

void InfluenceManager::update(std::vector<Unit*>* units) const {
	MapsUtils::resetMaps(unitsInfluencePerPlayer);
	if (SIM_GLOBALS.HEADLESS) {
		for (const auto unit : (*units)) {
			const auto pId = unit->getPlayer();
			const auto index = calculator->indexFromPosition(unit->getPosition());
			unitsInfluencePerPlayer[pId]->tempUpdate(index);
		}
	} else {
		MapsUtils::resetMaps(unitsNumberPerPlayer);
		for (const auto unit : (*units)) {
			const auto pId = unit->getPlayer();
			const auto index = calculator->indexFromPosition(unit->getPosition());
			unitsNumberPerPlayer[pId]->updateInt(index);
			unitsInfluencePerPlayer[pId]->tempUpdate(index);
		}
	}

	MapsUtils::finalize(unitsInfluencePerPlayer);
}

void InfluenceManager::update(const std::vector<ResourceEntity*>* resources) const {
	resourceInfluence->reset();
	for (const auto resource : (*resources)) {
		resourceInfluence->tempUpdate(resource->getIndexInInfluence(), resource->getHealthPercent());
	}
	//resourceInfluence->updateFromTemp();
}

void InfluenceManager::updateQuadUnits(const std::vector<Unit*>* units) const {
	MapsUtils::resetMaps(armyQuad);
	for (const auto unit : (*units)) {
		if (!unit->getDbUnit()->typeWorker) {
			armyQuad[unit->getPlayer()]->updateInt(unit);
		}
	}
}

void InfluenceManager::update(const std::vector<Building*>* buildings) const {
	MapsUtils::resetMaps(buildingsInfluencePerPlayer);
	MapsUtils::resetMaps(buildingsQuad);
	for (const auto building : (*buildings)) {
		const auto player = building->getPlayer();
		const auto index = building->getIndexInInfluence();
		buildingsInfluencePerPlayer[player]->tempUpdate(index);
		buildingsQuad[player]->updateInt(index);
	}
	MapsUtils::finalize(buildingsInfluencePerPlayer);
}

void InfluenceManager::updateWithHistory() const {
	MapsUtils::resetMaps(foodGatherSpeed);
	MapsUtils::resetMaps(woodGatherSpeed);
	MapsUtils::resetMaps(stoneGatherSpeed);
	MapsUtils::resetMaps(goldGatherSpeed);

	MapsUtils::resetMaps(attackSpeed);

	MapsUtils::finalize(foodGatherSpeed);
	MapsUtils::finalize(woodGatherSpeed);
	MapsUtils::finalize(stoneGatherSpeed);
	MapsUtils::finalize(goldGatherSpeed);

	MapsUtils::finalize(attackSpeed);
}

void InfluenceManager::updateQuadOther() const {
	MapsUtils::resetMaps(econQuad);
}

void InfluenceManager::updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
                                        std::vector<ResourceEntity*>* resources) const {
	visibilityManager->updateVisibility(buildings, units, resources);
}

void InfluenceManager::draw(InfluenceDataType type, char index) {
	DebugLineRepo::clear(DebugLineType::INFLUENCE, currentDebugBatch);
	DebugLineRepo::beginGeometry(DebugLineType::INFLUENCE, currentDebugBatch);

	switch (type) {
	case InfluenceDataType::NONE:
		break;
	case InfluenceDataType::UNITS_NUMBER_PER_PLAYER:
		MapsUtils::drawMap(currentDebugBatch, index, unitsNumberPerPlayer);
		break;
	case InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER:
		MapsUtils::drawMap(currentDebugBatch, index, unitsInfluencePerPlayer);
		break;
	case InfluenceDataType::BUILDING_INFLUENCE_PER_PLAYER:
		MapsUtils::drawMap(currentDebugBatch, index, buildingsInfluencePerPlayer);
		break;
	case InfluenceDataType::RESOURCE_INFLUENCE:
		resourceInfluence->draw(currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
		break;
	case InfluenceDataType::FOOD_SPEED:
		MapsUtils::drawMap(currentDebugBatch, index, foodGatherSpeed);
		break;
	case InfluenceDataType::WOOD_SPEED:
		MapsUtils::drawMap(currentDebugBatch, index, woodGatherSpeed);
		break;
	case InfluenceDataType::STONE_SPEED:
		MapsUtils::drawMap(currentDebugBatch, index, stoneGatherSpeed);
		break;
	case InfluenceDataType::GOLD_SPEED:
		MapsUtils::drawMap(currentDebugBatch, index, goldGatherSpeed);
		break;
	case InfluenceDataType::ATTACK_SPEED:
		MapsUtils::drawMap(currentDebugBatch, index, attackSpeed);
		break;
	// case InfluenceDataType::ECON_QUAD:
	// 	drawMap(index, econQuad);
	// 	break;
	// case InfluenceDataType::BUILDINGS_QUAD:
	// 	drawMap(index, buildingsQuad);
	// 	break;
	// case InfluenceDataType::UNITS_QUAD:
	// 	drawMap(index, armyQuad);
	// 	break;
	case InfluenceDataType::VISIBILITY:
		visibilityManager->drawMaps(currentDebugBatch, index);
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
	MapsUtils::drawAll(unitsNumberPerPlayer, "unitsInt");
	MapsUtils::drawAll(buildingsInfluencePerPlayer, "buildingsInt");
	MapsUtils::drawAll(unitsInfluencePerPlayer, "units");
	resourceInfluence->print("resource_");

	MapsUtils::drawAll(foodGatherSpeed, "gatherFood");
	MapsUtils::drawAll(woodGatherSpeed, "gatherWood");
	MapsUtils::drawAll(stoneGatherSpeed, "gatherStone");
	MapsUtils::drawAll(goldGatherSpeed, "gatherGold");
	MapsUtils::drawAll(attackSpeed, "attack");

	MapsUtils::drawAll(armyQuad, "armyQuad");
	MapsUtils::drawAll(buildingsQuad, "buildingsQuad");
	MapsUtils::drawAll(econQuad, "econQuad");
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
		array[i]->ensureReady();
		dataFromPos[i] = array[i]->getValueAsPercent(pos);
	}
	return dataFromPos;
}

std::vector<int> InfluenceManager::getIndexesIterative(const std::span<float> result, float tolerance, int min,
                                                       std::span<InfluenceMapFloat*> maps) const {
	assert(result.size() == maps.size());
	for (auto map : maps) {
		map->ensureReady();
	}
	int k = 0;
	for (auto step : {0.0f, 0.05f, 0.1f}) {
		tolerance += step;
		k++;
		std::vector<int> intersection = maps[0]->getIndexesWithByValue(result[0], tolerance);
		for (char i = 1; i < maps.size(); ++i) {
			std::vector<int> indexes = maps[i]->getIndexesWithByValue(result[i], tolerance);
			std::vector<int> temp;
			std::ranges::set_intersection(intersection, indexes,
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
	//TODO better!!!
	if (result.size() == 8) {
		return centersFromIndexes(getIndexesIterative(result, tolerance, min, mapsForAiPerPlayer[player]));
	}
	if (result.size() == 3) {
		return centersFromIndexes(getIndexesIterative(result, tolerance, min, mapsForAiArmyPerPlayer[player]));
	}
	assert(false);
}

std::vector<int>* InfluenceManager::getAreas(const std::span<float> result, char player) {
	auto& maps = mapsForAiPerPlayer[player];
	assert(result.size() == maps.size());

	std::fill_n(intersection, arraySize, 0.f); //TODO perf move to removeunsean
	const int noOfVisible = visibilityManager->removeUnseen(player, intersection);

	for (const auto map : maps) {
		map->ensureReady();
	}
	char numberOfNotEmptyMap = 0;
	for (char i = 0; i < maps.size(); ++i) {
		const auto ok = maps[i]->cumulateErros(result[i], intersection);
		numberOfNotEmptyMap += ok;
	}

	const int size = Urho3D::Min(noOfVisible, calculator->getResolution() * calculator->getResolution() / 8);
	const auto idx = sort_indexes(std::span(intersection, arraySize), size);

	return centersFromIndexes(intersection, idx, 0.1f * numberOfNotEmptyMap);
}

void InfluenceManager::addCollect(Unit* unit, char resId, float value) {
	const auto playerId = unit->getPlayer();

	assert(foodGatherSpeed[playerId]->getResolution() == calculator->getResolution());

	const auto index = calculator->indexFromPosition(unit->getPosition());
	switch (resId) {
	//TODO better!!!
	case 0:
		foodGatherSpeed[playerId]->tempUpdate(index, value);
		break;
	case 1:
		woodGatherSpeed[playerId]->tempUpdate(index, value);
		break;
	case 2:
		stoneGatherSpeed[playerId]->tempUpdate(index, value);
		break;
	case 3:
		goldGatherSpeed[playerId]->tempUpdate(index, value);
		break;
	}
	econQuad[playerId]->update(index, value);
}

void InfluenceManager::addAttack(char player, const Urho3D::Vector3& position, float value) {
	attackSpeed[player]->tempUpdate(position, value);
}

std::optional<Urho3D::Vector2> InfluenceManager::getCenterOf(CenterType id, char player) {
	return mapsForCentersPerPlayer[player][cast(id)]->getCenter();
}

bool InfluenceManager::isVisible(char player, const Urho3D::Vector2& pos) const {
	return visibilityManager->isVisible(player, pos);
}

Urho3D::Vector2 InfluenceManager::getCenter(int index) const {
	return calculator->getCenter(index);
}

float InfluenceManager::getVisibilityScore(char player) const {
	return visibilityManager->getVisibilityScore(player);
}

int InfluenceManager::getIndex(const Urho3D::Vector3& position) const {
	return calculator->indexFromPosition(position);
}

void InfluenceManager::nextVisibilityType() {
	visibilityManager->nextVisibilityType();
}

std::vector<int>* InfluenceManager::centersFromIndexes(float* values, const std::vector<unsigned>& indexes,
                                                       float minVal) const {
	tempIndexes->clear();

	for (auto ptr = indexes.begin(); (ptr < indexes.begin() + 256 && ptr < indexes.end()); ++ptr) {
		if (values[*ptr] > minVal) {
			break;
		}
		tempIndexes->emplace_back(*ptr);
	}
	return tempIndexes;
}

std::vector<Urho3D::Vector2> InfluenceManager::centersFromIndexes(const std::vector<int>& intersection) const {
	std::vector<Urho3D::Vector2> centers;
	centers.reserve(intersection.size());
	for (const auto value : intersection) {
		centers.emplace_back(calculator->getCenter(value));
	}
	return centers;
}

float InfluenceManager::getFieldSize() const {
	return resourceInfluence->getFieldSize();
}
