#include "InfluenceManager.h"
#include <exprtk/exprtk.hpp>
#include <ranges>

#include "CenterType.h"
#include "EconomicCenterUtils.h"
#include "InfluenceCenterUtils.h"
#include "MapsUtils.h"
#include "VisibilityManager.h"
#include "map/InfluenceMap.h"
#include "debug/DebugLineRepo.h"
#include "math/VectorUtils.h"
#include "env/bucket/CellEnums.h"
#include "objects/resource/ResourceEntity.h"
#include "env/GridCalculatorProvider.h"
#include "env/ContentInfo.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "utils/AssertUtils.h"
#include "utils/OtherUtils.h"
#include "utils/PrintUtils.h"
#include "utils/SpanUtils.h"

InfluenceManager::InfluenceManager(unsigned char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain) {
	buildingPresence.reserve(numberOfPlayers);
	unitPresence.reserve(numberOfPlayers);

	for (auto& gs : gatheringActivityByResource) {
		gs.reserve(numberOfPlayers);
	}

	for (auto& mapsByResource : unboostedResourceByResource) {
		mapsByResource.reserve(numberOfPlayers);
	}
	unboostedResource.reserve(numberOfPlayers);

	attackActivity.reserve(numberOfPlayers);
	armyPresence.reserve(numberOfPlayers);

	buildPlacementByPlayer.reserve(numberOfPlayers);
	unsigned short resolution = mapSize / INF_GRID_FIELD_SIZE;
	calculator = GridCalculatorProvider::get(resolution, mapSize);
	unsigned int quadArraySize = 0;
	auto currentRes = resolution;
	while (currentRes % 2 == 0 && currentRes != 1) {
		currentRes /= 2;
	}
	currentRes *= 2;
	for (int i = currentRes; i < resolution; i *= 2) {
		quadArraySize += i * i;
		quadResolutions.push_back(i);
	}
	quadValues.resize(quadArraySize);
	economicRawValues.resize(resolution * resolution);
	economicCenters.resize(numberOfPlayers);
	auto offset = 0u;
	for (const auto quadResolution : quadResolutions) {
		const auto layerSize = quadResolution * quadResolution;
		quadLayers.emplace_back(quadValues.data() + offset, layerSize);
		offset += layerSize;
	}
	assert(!quadLayers.empty());
	for (int player = 0; player < numberOfPlayers; ++player) {
		buildingPresence.emplace_back(new InfluenceMap(calculator, 2.f));
		unitPresence.emplace_back(new InfluenceMap(calculator));

		for (auto& gs : gatheringActivityByResource) {
			gs.emplace_back(new InfluenceMap(calculator, 40.f, true));
		}

		for (auto& mapsByResource : unboostedResourceByResource) {
			mapsByResource.emplace_back(new InfluenceMap(calculator, 5.f));
		}

		attackActivity.emplace_back(new InfluenceMap(calculator, 40.f, true));
		armyPresence.emplace_back(new InfluenceMap(calculator));
	}

	for (int player = 0; player < numberOfPlayers; ++player) {
		const int enemy = 1 - player;
		std::array<InfluenceMap*, RESOURCES_SIZE> gatheringForPlayer;
		std::array<InfluenceMap*, RESOURCES_SIZE> unboostedForPlayer;
		for (int r = 0; r < RESOURCES_SIZE; ++r) {
			gatheringForPlayer[r] = gatheringActivityByResource[r][player];
			unboostedForPlayer[r] = unboostedResourceByResource[r][player];
		}
		unboostedResource.emplace_back(new InfluenceMap(calculator, 5.f));

		buildPlacementByPlayer.emplace_back(std::array<InfluenceMap*, AI_MAP_COUNT>{
					                                buildingPresence[player],
					                                unitPresence[player],
					                                attackActivity[player],
					                                gatheringForPlayer[cast(ResourceType::FOOD)],
					                                gatheringForPlayer[cast(ResourceType::WOOD)],
					                                gatheringForPlayer[cast(ResourceType::STONE)],
					                                gatheringForPlayer[cast(ResourceType::GOLD)],
					                                buildingPresence[enemy],
					                                unitPresence[enemy],
				                                attackActivity[enemy],
				                                unboostedResource[player],
				                        });
		unboostedResourceByPlayer.emplace_back(unboostedForPlayer);
		assert(validSizes(buildPlacementByPlayer.at(player)));
	}
	visibilityManager = new VisibilityManager(numberOfPlayers, mapSize, terrain);

	ci = new content_info();
	DebugLineRepo::init(DebugLineType::INFLUENCE, MAX_DEBUG_PARTS_INFLUENCE);

	arraySize = calculator->getResolution() * calculator->getResolution();
	assert(arraySize <= std::numeric_limits<unsigned short>::max());
	errorsSum.resize(arraySize);

	assert(calculator->getResolution() == unitPresence[0]->getResolution());
}

InfluenceManager::~InfluenceManager() {
	clear_vector(buildingPresence);
	clear_vector(unitPresence);
	for (auto& vec : gatheringActivityByResource) {
		clear_vector(vec);
	}
	for (auto& vec : unboostedResourceByResource) {
		clear_vector(vec);
	}
	clear_vector(unboostedResource);

	clear_vector(attackActivity);

	clear_vector(armyPresence);

	delete visibilityManager;
	delete ci;
}

void InfluenceManager::updateUnits(std::vector<Unit*>* units) const {
	MapsUtils::resetMaps(unitPresence);
	MapsUtils::resetMaps(armyPresence);
	for (auto& vec : unboostedResourceByResource) {
		MapsUtils::resetMaps(vec);
	}
	MapsUtils::resetMaps(unboostedResource);

	for (const auto unit : (*units)) {
		const auto pId = unit->getPlayer();
		const auto index = getIndexInInfluence(unit);
		unitPresence[pId]->update(index);
		if (!unit->getDb()->typeWorker) {
			armyPresence[pId]->update(index);
		} else if (unit->getState() == UnitState::COLLECT && unit->isFirstThingAlive()) {
			auto res = static_cast<ResourceEntity*>(unit->getThingToInteract());
			// TODO albo uzyc occupied cell tylko trzeba jakos przeliczyc
			unboostedResourceByPlayer[unit->getPlayer()][res->getResourceId()]->update(res->getIndexInInfluence());
			unboostedResource[unit->getPlayer()]->update(res->getIndexInInfluence());
		}
	}
	
	//TODO perf to tutaj chyba niepotrzebne i tak bedzie zrobione lazy
	//MapsUtils::finalize(unitPresence);
}

void InfluenceManager::updateBuildings(const std::vector<Building*>* buildings) const {
	MapsUtils::resetMaps(buildingPresence);
	for (const auto building : (*buildings)) {
		buildingPresence[building->getPlayer()]->update(building->getIndexInInfluence());
	}
	// TODO perf to tutaj chyba niepotrzebne i tak bedzie zrobione lazy
	//MapsUtils::finalize(buildingPresence);
}

void InfluenceManager::updateWithHistory() const {
	for (auto& vec : gatheringActivityByResource) {
		MapsUtils::resetMaps(vec);
		MapsUtils::finalize(vec);
	}

	MapsUtils::resetMaps(attackActivity);//obniza wartosci
	MapsUtils::finalize(attackActivity);//dopisuje nowe
	for (auto& cache : economicCenters) {
		cache.dirty = true;
	}
	economicRawPlayer.reset();
}

void InfluenceManager::updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
                                        std::vector<ResourceEntity*>* resources) const {
	visibilityManager->updateVisibility(buildings, units, resources);
}

void InfluenceManager::resetHistoryThresholds() const {
	for (auto& vec : gatheringActivityByResource) {
		MapsUtils::resetToZeroMaps(vec);
	}
	MapsUtils::resetToZeroMaps(attackActivity);
	for (auto& cache : economicCenters) {
		cache.dirty = true;
	}
	economicRawPlayer.reset();
}

void InfluenceManager::draw(EnvironmentDebugMode mode, unsigned char index) {
	DebugLineRepo::clear(DebugLineType::INFLUENCE, currentDebugBatch);
	DebugLineRepo::beginGeometry(DebugLineType::INFLUENCE, currentDebugBatch);

	switch (mode) {
	case EnvironmentDebugMode::NONE:
	case EnvironmentDebugMode::MAIN_GRID:
		break;
	case EnvironmentDebugMode::UNITS_RAW:
		MapsUtils::drawMapRaw(currentDebugBatch, index, unitPresence);
		break;
	case EnvironmentDebugMode::UNITS_INFLUENCE:
		MapsUtils::drawMapKernel(currentDebugBatch, index, unitPresence);
		break;
	case EnvironmentDebugMode::BUILDING_INFLUENCE:
		MapsUtils::drawMapKernel(currentDebugBatch, index, buildingPresence);
		break;
	case EnvironmentDebugMode::FOOD_SPEED:
		MapsUtils::drawMapKernel(currentDebugBatch, index, gatheringActivityByResource[cast(ResourceType::FOOD)]);
		break;
	case EnvironmentDebugMode::WOOD_SPEED:
		MapsUtils::drawMapKernel(currentDebugBatch, index, gatheringActivityByResource[cast(ResourceType::WOOD)]);
		break;
	case EnvironmentDebugMode::STONE_SPEED:
		MapsUtils::drawMapKernel(currentDebugBatch, index, gatheringActivityByResource[cast(ResourceType::STONE)]);
		break;
	case EnvironmentDebugMode::GOLD_SPEED:
		MapsUtils::drawMapKernel(currentDebugBatch, index, gatheringActivityByResource[cast(ResourceType::GOLD)]);
		break;
	case EnvironmentDebugMode::ATTACK_SPEED:
		MapsUtils::drawMapKernel(currentDebugBatch, index, attackActivity);
		break;
	case EnvironmentDebugMode::ECONOMY:
		index %= gatheringActivityByResource[0].size();
		rebuildEconomicRaw(index);
		gatheringActivityByResource[0][index]->drawRaw(economicRawValues, currentDebugBatch, MAX_DEBUG_PARTS_INFLUENCE);
		break;
	case EnvironmentDebugMode::VISIBILITY:
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
	for (int player = 0; player < buildingPresence.size(); ++player) {
		printMapWithQuads(*buildingPresence[player], "buildingPresence_" + Urho3D::String(player) + "_");
		printMapWithQuads(*unitPresence[player], "unitPresence_" + Urho3D::String(player) + "_");
	}

	constexpr const char* gatheringActivityNames[] = {
		"gatheringActivityFood", "gatheringActivityWood", "gatheringActivityStone", "gatheringActivityGold"};
	for (int r = 0; r < RESOURCES_SIZE; ++r) {
		for (int player = 0; player < gatheringActivityByResource[r].size(); ++player) {
			printMapWithQuads(*gatheringActivityByResource[r][player],
			                  Urho3D::String(gatheringActivityNames[r]) + "_" + Urho3D::String(player) + "_");
		}
	}
	for (int player = 0; player < attackActivity.size(); ++player) {
		printMapWithQuads(*attackActivity[player], "attackActivity_" + Urho3D::String(player) + "_");
		printMapWithQuads(*armyPresence[player], "armyPresence_" + Urho3D::String(player) + "_");
		printEconomicWithQuads(player);
	}
}

content_info* InfluenceManager::getContentInfo(const Urho3D::Vector2& center, CellState state, int additionalInfo,
                                               bool checks[], int activePlayer) const {
	ci->reset(); //TODO przemyslec to, zbyt skomplikowane
	switch (state) {
	case CellState::NONE:
	case CellState::ATTACK:
	case CellState::COLLECT:
	case CellState::DEPLOY:
		if (checks[3] || checks[4]) {
			for (int i = 0; i < unitPresence.size(); ++i) {
				const auto value = unitPresence[i]->getRaw(center);
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



const std::vector<unsigned>&
InfluenceManager::getAreas(std::span<const float> result, unsigned char player) const {
	std::array<InfluenceMap*, AI_MAP_COUNT> maps = buildPlacementByPlayer[player];
	return getBestVisibleIndexes(maps, result, player);
}

std::vector<unsigned> InfluenceManager::getAreasResBonus(unsigned char id, unsigned char player) const {
	return unboostedResourceByPlayer[player][id]->getRawMaxIdxs();
}

const std::vector<unsigned>&
InfluenceManager::getBestVisibleIndexes(std::span<InfluenceMap*> maps, std::span<const float> result, unsigned char player) const {
	assert(result.size() == maps.size());

	//unseen means max float max error
	const int noOfVisible = visibilityManager->removeUnseen(player, std::span<float>(errorsSum));

	char numberOfNotEmptyMap = 0;
	for (auto&& [map, value] : std::views::zip(maps, result)) {
		numberOfNotEmptyMap += map->cumulateErrors(value, std::span<float>(errorsSum));
	}

	const int size = Urho3D::Min(noOfVisible, arraySize / 8);
	
	collectSortedBelow(tempIndexes, std::span<float>(errorsSum), 10.f * numberOfNotEmptyMap, size);
	return tempIndexes;
}

int InfluenceManager::getIndexInInfluence(Unit* unit) const {
	if (unit->getIndexInInfluence() == -1) {
		const auto newIndex = calculator->indexFromPosition(unit->getPosition());
		unit->setIndexInInfluence(newIndex);
	}
	return unit->getIndexInInfluence();
}

void InfluenceManager::addCollect(Unit* unit, short resId, float value) const {
	const auto playerId = unit->getPlayer();

	assert(gatheringActivityByResource[cast(ResourceType::FOOD)][playerId]->getResolution() == calculator->getResolution());
	const auto index = getIndexInInfluence(unit);
	gatheringActivityByResource[resId][playerId]->update(index, value);

}

void InfluenceManager::addAttack(unsigned char player, const Urho3D::Vector2& position, float value) const {
	attackActivity[player]->update(position, value);
}

std::optional<Urho3D::Vector2> InfluenceManager::getCenterOf(CenterType id, unsigned char player) const {
	switch (id) {
	case CenterType::ECON:
		return getEconomicCenter(player);
	case CenterType::BUILDING:
		ensureCenter(*buildingPresence[player]);
		return buildingPresence[player]->center;
	case CenterType::ARMY:
		ensureCenter(*armyPresence[player]);
		return armyPresence[player]->center;
	case CenterType::BATTLE:
		ensureCenter(*attackActivity[player]);
		return attackActivity[player]->center;
	}

	assert(false);
	return std::nullopt;
}

void InfluenceManager::rebuildEconomicRaw(unsigned char player) const {
	if (economicRawPlayer == player && !economicCenters[player].dirty) {
		return;
	}

	std::array<std::span<const float>, RESOURCES_SIZE> rawValues;
	for (int resource = 0; resource < RESOURCES_SIZE; ++resource) {
		const auto* map = gatheringActivityByResource[resource][player];
		rawValues[resource] = std::span<const float>(map->rawValues, map->arraySize);
	}
	EconomicCenterUtils::sumRawValues(economicRawValues, rawValues);
	economicRawPlayer = player;
}

std::optional<Urho3D::Vector2> InfluenceManager::getEconomicCenter(unsigned char player) const {
	auto& cache = economicCenters[player];
	if (!cache.dirty) {
		return cache.center;
	}

	rebuildEconomicRaw(player);
	ensureCenter(economicRawValues, cache.center, cache.dirty);
	return cache.center;
}

void InfluenceManager::ensureCenter(std::span<const float> rawValues, std::optional<Urho3D::Vector2>& center,
                                    bool& centerDirty) const {
	if (!centerDirty) {
		return;
	}

	InfluenceCenterUtils::rebuildQuad(rawValues, calculator->getResolution(), quadLayers, quadResolutions);
	const auto index = InfluenceCenterUtils::findCenterIndex(rawValues, quadLayers, quadResolutions);
	if (!index.has_value()) {
		center.reset();
		centerDirty = false;
		return;
	}

	center = calculator->getCenter(*index);
	centerDirty = false;
}

void InfluenceManager::ensureCenter(InfluenceMap& map) const {
	ensureCenter(std::span<const float>(map.rawValues, map.arraySize), map.center, map.centerDirty);
}

void InfluenceManager::printMapWithQuads(InfluenceMap& map, const Urho3D::String& name) const {
	map.print(name);
	InfluenceCenterUtils::rebuildQuad(std::span<const float>(map.rawValues, map.arraySize), calculator->getResolution(),
	                                  quadLayers, quadResolutions);
	for (int i = 0; i < static_cast<int>(quadLayers.size()); ++i) {
		map.printMap(quadLayers[i], name + "_quad_" + Urho3D::String(i));
	}
}

void InfluenceManager::printEconomicWithQuads(unsigned char player) const {
	rebuildEconomicRaw(player);
	auto* map = gatheringActivityByResource[0][player];
	const auto playerName = Urho3D::String(static_cast<int>(player));
	map->printMap(economicRawValues, "gatherSpeedCombined_" + playerName + "_raw");
	InfluenceCenterUtils::rebuildQuad(economicRawValues, calculator->getResolution(), quadLayers, quadResolutions);
	for (int i = 0; i < static_cast<int>(quadLayers.size()); ++i) {
		map->printMap(quadLayers[i], "gatherSpeedCombined_" + playerName + "_quad_" + Urho3D::String(i));
	}
}

bool InfluenceManager::isVisible(unsigned char player, const Urho3D::Vector2& pos) const {
	return visibilityManager->isVisible(player, pos);
}

Urho3D::Vector2 InfluenceManager::getCenter(int index) const {
	return calculator->getCenter(index);
}

float InfluenceManager::getVisibilityScore(unsigned char player) const {
	return visibilityManager->getVisibilityScore(player);
}

int InfluenceManager::getIndex(const Urho3D::Vector2& position) const {
	return calculator->indexFromPosition(position);
}

void InfluenceManager::nextVisibilityType() const {
	visibilityManager->nextVisibilityType();
}

float InfluenceManager::getFieldSize() const {
	return calculator->getFieldSize();
}
