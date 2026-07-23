#include "InfluenceManager.h"
#include <exprtk/exprtk.hpp>
#include <magic_enum.hpp>
#include <ranges>

#include "CenterType.h"
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

static_assert(magic_enum::enum_count<CenterType>() == CENTER_TYPE_COUNT,
              "CENTER_TYPE_COUNT must equal the number of CenterType enumerators");

InfluenceManager::InfluenceManager(unsigned char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain) {
	buildingPresence.reserve(numberOfPlayers);
	unitPresence.reserve(numberOfPlayers);

	for (auto& gs : gatheringActivityByResource) {
		gs.reserve(numberOfPlayers);
	}

	for (auto& mapsByResource : unboostedResourceActivityByTypeAndPlayer) {
		mapsByResource.reserve(numberOfPlayers);
	}
	unboostedResourceActivityByPlayer.reserve(numberOfPlayers);

	attackActivity.reserve(numberOfPlayers);
	armyPresence.reserve(numberOfPlayers);
	economicActivity.reserve(numberOfPlayers);

	buildPlacementByPlayer.reserve(numberOfPlayers);
	centerSourceByPlayer.reserve(numberOfPlayers);
	unsigned short resolution = mapSize / INF_GRID_FIELD_SIZE;
	calculator = GridCalculatorProvider::get(resolution, mapSize);
	for (int player = 0; player < numberOfPlayers; ++player) {
		buildingPresence.emplace_back(new InfluenceMap(calculator, 2.f));
		unitPresence.emplace_back(new InfluenceMap(calculator));

		for (auto& gs : gatheringActivityByResource) {
			gs.emplace_back(new InfluenceMap(calculator, 40.f, true));
		}

		for (auto& mapsByResource : unboostedResourceActivityByTypeAndPlayer) {
			mapsByResource.emplace_back(new InfluenceMap(calculator, 5.f));
		}

		attackActivity.emplace_back(new InfluenceMap(calculator, 40.f, true));
		armyPresence.emplace_back(new InfluenceMap(calculator));
		economicActivity.emplace_back(new InfluenceMap(calculator, 40.f, true));
	}

	for (int player = 0; player < numberOfPlayers; ++player) {
		const int enemy = 1 - player;
		std::array<InfluenceMap*, RESOURCES_SIZE> gatheringForPlayer;
		std::array<InfluenceMap*, RESOURCES_SIZE> unboostedForPlayer;
		for (int r = 0; r < RESOURCES_SIZE; ++r) {
			gatheringForPlayer[r] = gatheringActivityByResource[r][player];
			unboostedForPlayer[r] = unboostedResourceActivityByTypeAndPlayer[r][player];
		}
		unboostedResourceActivityByPlayer.emplace_back(new InfluenceMap(calculator, 5.f));

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
				                                unboostedResourceActivityByPlayer[player],
				                        });
		centerSourceByPlayer.emplace_back(std::array<InfluenceMap*, CENTER_TYPE_COUNT>{
			                             economicActivity[player],
			                             buildingPresence[player],
			                             armyPresence[player],
			                             attackActivity[player]
			                             });
		unboostedResourceActivityByPlayerAndType.emplace_back(unboostedForPlayer);
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
	for (auto& vec : unboostedResourceActivityByTypeAndPlayer) {
		clear_vector(vec);
	}
	clear_vector(unboostedResourceActivityByPlayer);

	clear_vector(attackActivity);

	clear_vector(armyPresence);
	clear_vector(economicActivity);

	delete visibilityManager;
	delete ci;
}

void InfluenceManager::updateUnits(std::vector<Unit*>* units) const {
	MapsUtils::resetMaps(unitPresence);
	MapsUtils::resetMaps(armyPresence);
	for (auto& vec : unboostedResourceActivityByTypeAndPlayer) {
		MapsUtils::resetMaps(vec);
	}
	MapsUtils::resetMaps(unboostedResourceActivityByPlayer);

	for (const auto unit : (*units)) {
		const auto pId = unit->getPlayer();
		const auto index = getIndexInInfluence(unit);
		unitPresence[pId]->update(index);
		if (!unit->getDb()->typeWorker) {
			armyPresence[pId]->update(index);
		} else if (unit->getState() == UnitState::COLLECT && unit->isFirstThingAlive()) {
			auto res = static_cast<ResourceEntity*>(unit->getThingToInteract());
			if (res->getBonus(pId) <= 1.f) {
				// TODO albo uzyc occupied cell tylko trzeba jakos przeliczyc
				unboostedResourceActivityByPlayerAndType[pId][res->getResourceId()]->update(res->getIndexInInfluence());
				unboostedResourceActivityByPlayer[pId]->update(res->getIndexInInfluence());
			}
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
	MapsUtils::resetMaps(economicActivity);

	MapsUtils::resetMaps(attackActivity);//obniza wartosci
	MapsUtils::finalize(attackActivity);//dopisuje nowe
}

void InfluenceManager::updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
                                        std::vector<ResourceEntity*>* resources) const {
	visibilityManager->updateVisibility(buildings, units, resources);
}

void InfluenceManager::resetHistoryThresholds() const {
	for (auto& vec : gatheringActivityByResource) {
		MapsUtils::resetToZeroMaps(vec);
	}
	MapsUtils::resetToZeroMaps(economicActivity);
	MapsUtils::resetToZeroMaps(attackActivity);
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
		MapsUtils::drawMapRaw(currentDebugBatch, index, economicActivity);
		break;
	case EnvironmentDebugMode::UNBOOSTED_RESOURCES:
		MapsUtils::drawMapRaw(currentDebugBatch, index, unboostedResourceActivityByPlayer);
		break;
	case EnvironmentDebugMode::UNBOOSTED_FOOD:
		MapsUtils::drawMapRaw(currentDebugBatch, index,
		                      unboostedResourceActivityByTypeAndPlayer[cast(ResourceType::FOOD)]);
		break;
	case EnvironmentDebugMode::UNBOOSTED_WOOD:
		MapsUtils::drawMapRaw(currentDebugBatch, index,
		                      unboostedResourceActivityByTypeAndPlayer[cast(ResourceType::WOOD)]);
		break;
	case EnvironmentDebugMode::UNBOOSTED_STONE:
		MapsUtils::drawMapRaw(currentDebugBatch, index,
		                      unboostedResourceActivityByTypeAndPlayer[cast(ResourceType::STONE)]);
		break;
	case EnvironmentDebugMode::UNBOOSTED_GOLD:
		MapsUtils::drawMapRaw(currentDebugBatch, index,
		                      unboostedResourceActivityByTypeAndPlayer[cast(ResourceType::GOLD)]);
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
	MapsUtils::drawAll(buildingPresence, "buildingPresence");
	MapsUtils::drawAll(unitPresence, "unitPresence");

	constexpr const char* gatheringActivityNames[] = {
		"gatheringActivityFood", "gatheringActivityWood", "gatheringActivityStone", "gatheringActivityGold"};
	for (int r = 0; r < RESOURCES_SIZE; ++r) {
		MapsUtils::drawAll(gatheringActivityByResource[r], gatheringActivityNames[r]);
	}
	MapsUtils::drawAll(attackActivity, "attackActivity");
	MapsUtils::drawAll(armyPresence, "armyPresence");
	MapsUtils::drawAll(economicActivity, "economicActivity");
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
	return unboostedResourceActivityByPlayerAndType[player][id]->getRawMaxIdxs();
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
	economicActivity[playerId]->update(index, value);
}

void InfluenceManager::addAttack(unsigned char player, const Urho3D::Vector2& position, float value) const {
	attackActivity[player]->update(position, value);
}

std::optional<Urho3D::Vector2> InfluenceManager::getCenterOf(CenterType id, unsigned char player) const {
	return centerSourceByPlayer[player][castC(id)]->getCenter();
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
