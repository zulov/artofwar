#include "InfluenceManager.h"
#include <exprtk/exprtk.hpp>
#include <magic_enum.hpp>
#include <ranges>

#include "CenterType.h"
#include "MapsUtils.h"
#include "VisibilityManager.h"
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

// mapsForCentersPerPlayer is sized CENTER_TYPE_COUNT and indexed by CenterType.
static_assert(magic_enum::enum_count<CenterType>() == CENTER_TYPE_COUNT,
              "CENTER_TYPE_COUNT must equal the number of CenterType enumerators");


InfluenceManager::InfluenceManager(unsigned char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain) {
	buildingsInfluencePerPlayer.reserve(numberOfPlayers);
	unitsInfluencePerPlayer.reserve(numberOfPlayers);

	for (auto& gs : gatherSpeed) {
		gs.reserve(numberOfPlayers);
	}

	for (auto& resNotInBonus : resNotInBonus) {
		resNotInBonus.reserve(numberOfPlayers);
	}

	attackSpeed.reserve(numberOfPlayers);
	armyQuad.reserve(numberOfPlayers);
	econQuad.reserve(numberOfPlayers);

	mapsForAiArmyPerPlayer.reserve(numberOfPlayers);
	mapsForAiPerPlayer.reserve(numberOfPlayers);
	mapsForCentersPerPlayer.reserve(numberOfPlayers);
	unsigned short resolution = mapSize / INF_GRID_FIELD_SIZE;
  sharedTemplateV = InfluenceMap::createTemplateV(0.5f, INF_LEVEL);
	for (int player = 0; player < numberOfPlayers; ++player) {
		buildingsInfluencePerPlayer.emplace_back(
	                                         new InfluenceMap(resolution, mapSize, 0.5f, INF_LEVEL, 2, sharedTemplateV));
		unitsInfluencePerPlayer.emplace_back(
	                                     new InfluenceMap(resolution, mapSize, 0.5f, INF_LEVEL, 40, sharedTemplateV));

		for (auto& gs : gatherSpeed) {
			gs.emplace_back(new InfluenceMap(resolution, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40, sharedTemplateV));
		}

		for (auto& resNotInBonus : resNotInBonus) {
			resNotInBonus.emplace_back(new InfluenceMap(resolution, mapSize, 0.5f, INF_LEVEL, 5, sharedTemplateV, false));
		}

		attackSpeed.emplace_back(new InfluenceMap(resolution, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40, sharedTemplateV));
		armyQuad.emplace_back(new InfluenceMap(resolution, mapSize, 0.5f, INF_LEVEL, 40, sharedTemplateV, false));
		econQuad.emplace_back(new InfluenceMap(mapSize / INF_GRID_FIELD_SIZE, mapSize, 0.5f, INF_LEVEL, 40, sharedTemplateV, false));
	}

	for (int player = 0; player < numberOfPlayers; ++player) {
		const int enemy = 1 - player;
		std::array<InfluenceMap*, RESOURCES_SIZE> gatherSpeedView;
		std::array<InfluenceMap*, RESOURCES_SIZE> resNotInBonusView;
		for (int r = 0; r < RESOURCES_SIZE; ++r) {
			gatherSpeedView[r] = gatherSpeed[r][player];
			resNotInBonusView[r] = resNotInBonus[r][player];
		}

		mapsForAiPerPlayer.emplace_back(std::array<InfluenceMap*, AI_MAP_COUNT>{
			                                buildingsInfluencePerPlayer[player],
			                                unitsInfluencePerPlayer[player],
			                                attackSpeed[player],
			                                gatherSpeedView[cast(ResourceType::FOOD)],
			                                gatherSpeedView[cast(ResourceType::WOOD)],
			                                gatherSpeedView[cast(ResourceType::STONE)],
			                                gatherSpeedView[cast(ResourceType::GOLD)],
			                                buildingsInfluencePerPlayer[enemy],
			                                unitsInfluencePerPlayer[enemy],
			                                attackSpeed[enemy],
		                                });
		mapsForAiArmyPerPlayer.emplace_back(std::array<InfluenceMap*, AI_ARMY_MAP_COUNT>{
			                                    buildingsInfluencePerPlayer[player],
			                                    unitsInfluencePerPlayer[player],
			                                    attackSpeed[player],
			                                    buildingsInfluencePerPlayer[enemy],
			                                    unitsInfluencePerPlayer[enemy],
			                                    attackSpeed[enemy],
		                                    });
		mapsForCentersPerPlayer.emplace_back(std::array<InfluenceMap*, CENTER_TYPE_COUNT>{
				                             econQuad[player],
				                             buildingsInfluencePerPlayer[player],
				                             armyQuad[player]
			                             });

		mapsGatherSpeedPerPlayer.emplace_back(gatherSpeedView);
		mapsResNotInBonusPerPlayer.emplace_back(resNotInBonusView);
		assert(validSizes(mapsForAiPerPlayer.at(player)));
	}
	visibilityManager = new VisibilityManager(numberOfPlayers, mapSize, terrain);

	calculator = GridCalculatorProvider::get(resolution, mapSize);
	ci = new content_info();
	DebugLineRepo::init(DebugLineType::INFLUENCE, MAX_DEBUG_PARTS_INFLUENCE);

	arraySize = calculator->getResolution() * calculator->getResolution();
	assert(arraySize <= std::numeric_limits<unsigned short>::max());
	errorsSum = new float[arraySize];

	assert(calculator->getResolution() == unitsInfluencePerPlayer[0]->getResolution());
}

InfluenceManager::~InfluenceManager() {
	clear_vector(buildingsInfluencePerPlayer);
	clear_vector(unitsInfluencePerPlayer);
	for (auto& vec : gatherSpeed) {
		clear_vector(vec);
	}
	for (auto& vec : resNotInBonus) {
		clear_vector(vec);
	}

	clear_vector(attackSpeed);

	clear_vector(armyQuad);
	clear_vector(econQuad);

	delete visibilityManager;
	delete ci;

	delete[]errorsSum;
	delete[] sharedTemplateV;
}

void InfluenceManager::updateUnits(std::vector<Unit*>* units) const {
	MapsUtils::resetMaps(unitsInfluencePerPlayer);
	MapsUtils::resetMaps(armyQuad);
	for (auto& vec : resNotInBonus) {
		MapsUtils::resetMaps(vec);
	}

	if (SIM_GLOBALS.HEADLESS) {
		for (const auto unit : (*units)) {
			const auto pId = unit->getPlayer();
			const auto index = getIndexInInfluence(unit);
			unitsInfluencePerPlayer[pId]->update(index);
			if (!unit->getDb()->typeWorker) {
				armyQuad[pId]->update(index);
			} else if (unit->getState() == UnitState::COLLECT && unit->isFirstThingAlive()) {
				auto res = static_cast<ResourceEntity*>(unit->getThingToInteract());
				// TODO albo uzyc occupied cell tylko trzeba jakos przeliczyc
				mapsResNotInBonusPerPlayer[unit->getPlayer()][res->getResourceId()]->update(res->getIndexInInfluence());
			}
		}
	} else {
		for (const auto unit : (*units)) {
			const auto pId = unit->getPlayer();
			const auto index = getIndexInInfluence(unit);
			unitsInfluencePerPlayer[pId]->update(index);
			if (!unit->getDb()->typeWorker) {
				armyQuad[pId]->update(index);
			} else if (unit->getState() == UnitState::COLLECT && unit->isFirstThingAlive()) {
				auto res = static_cast<ResourceEntity*>(unit->getThingToInteract());
				// TODO albo uzyc occupied cell tylko trzeba jakos przeliczyc
				mapsResNotInBonusPerPlayer[unit->getPlayer()][res->getResourceId()]->update(res->getIndexInInfluence());
			}

		}
	}	
	//TODO perf to tutaj chyba niepotrzebne i tak bedzie zrobione lazy
	//MapsUtils::finalize(unitsInfluencePerPlayer);
}

void InfluenceManager::updateBuildings(const std::vector<Building*>* buildings) const {
	MapsUtils::resetMaps(buildingsInfluencePerPlayer);
	for (const auto building : (*buildings)) {
		const auto player = building->getPlayer();
		const auto index = building->getIndexInInfluence();
		buildingsInfluencePerPlayer[player]->update(index);
	}
	// TODO perf to tutaj chyba niepotrzebne i tak bedzie zrobione lazy
	//MapsUtils::finalize(buildingsInfluencePerPlayer);
}

void InfluenceManager::updateWithHistory() const {
	for (auto& vec : gatherSpeed) {
		MapsUtils::resetMaps(vec);
		MapsUtils::finalize(vec);
	}

	MapsUtils::resetMaps(attackSpeed);//obniza wartosci
	MapsUtils::finalize(attackSpeed);//dopisuje nowe
}

void InfluenceManager::updateQuadOther() const {
	MapsUtils::resetMaps(econQuad);
}

void InfluenceManager::updateVisibility(std::vector<Building*>* buildings, std::vector<Unit*>* units,
                                        std::vector<ResourceEntity*>* resources) const {
	visibilityManager->updateVisibility(buildings, units, resources);
}

void InfluenceManager::resetHistoryThresholds() const {
	for (auto& vec : gatherSpeed) {
		MapsUtils::resetToZeroMaps(vec);
	}
	MapsUtils::resetToZeroMaps(attackSpeed);
}

void InfluenceManager::draw(InfluenceDataType type, unsigned char index) {
	DebugLineRepo::clear(DebugLineType::INFLUENCE, currentDebugBatch);
	DebugLineRepo::beginGeometry(DebugLineType::INFLUENCE, currentDebugBatch);

	switch (type) {
	case InfluenceDataType::NONE:
		break;
	case InfluenceDataType::UNITS_NUMBER_PER_PLAYER:
		MapsUtils::drawMapRaw(currentDebugBatch, index, unitsInfluencePerPlayer);
		break;
	case InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER:
		MapsUtils::drawMapKernel(currentDebugBatch, index, unitsInfluencePerPlayer);
		break;
	case InfluenceDataType::BUILDING_INFLUENCE_PER_PLAYER:
		MapsUtils::drawMapKernel(currentDebugBatch, index, buildingsInfluencePerPlayer);
		break;
	case InfluenceDataType::FOOD_SPEED:
		MapsUtils::drawMapKernel(currentDebugBatch, index, gatherSpeed[cast(ResourceType::FOOD)]);
		break;
	case InfluenceDataType::WOOD_SPEED:
		MapsUtils::drawMapKernel(currentDebugBatch, index, gatherSpeed[cast(ResourceType::WOOD)]);
		break;
	case InfluenceDataType::STONE_SPEED:
		MapsUtils::drawMapKernel(currentDebugBatch, index, gatherSpeed[cast(ResourceType::STONE)]);
		break;
	case InfluenceDataType::GOLD_SPEED:
		MapsUtils::drawMapKernel(currentDebugBatch, index, gatherSpeed[cast(ResourceType::GOLD)]);
		break;
	case InfluenceDataType::ATTACK_SPEED:
		MapsUtils::drawMapKernel(currentDebugBatch, index, attackSpeed);
		break;
	case InfluenceDataType::ECON_QUAD:
		MapsUtils::drawMapRaw(currentDebugBatch, index, econQuad);
		break;
	case InfluenceDataType::BUILDINGS_QUAD:
		MapsUtils::drawMapRaw(currentDebugBatch, index, buildingsInfluencePerPlayer);
		break;
	case InfluenceDataType::UNITS_QUAD:
		MapsUtils::drawMapRaw(currentDebugBatch, index, armyQuad);
		break;
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
	MapsUtils::drawAll(buildingsInfluencePerPlayer, "buildingsInt");
	MapsUtils::drawAll(unitsInfluencePerPlayer, "units");

	constexpr const char* gatherNames[] = {"gatherFood", "gatherWood", "gatherStone", "gatherGold"};
	for (int r = 0; r < RESOURCES_SIZE; ++r) {
		MapsUtils::drawAll(gatherSpeed[r], gatherNames[r]);
	}
	MapsUtils::drawAll(attackSpeed, "attack");

	MapsUtils::drawAll(armyQuad, "armyQuad");
	MapsUtils::drawAll(econQuad, "econQuad");
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
			for (int i = 0; i < unitsInfluencePerPlayer.size(); ++i) {
				const auto value = unitsInfluencePerPlayer[i]->getRaw(center);
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



const std::vector<Urho3D::Vector2>&
InfluenceManager::getBestVisibleAreas(std::span<const float> result, unsigned char player) {
	if (result.size() == AI_MAP_COUNT) {
		return centersFromIndexes(getBestVisibleIndexes(mapsForAiPerPlayer[player], result, player));
	}
	if (result.size() == AI_ARMY_MAP_COUNT) {
		return centersFromIndexes(getBestVisibleIndexes(mapsForAiArmyPerPlayer[player], result, player));
	}
	assert(false);
	tempCenters.clear();
	return tempCenters; // unreachable in debug; avoids UB on unexpected span size in release
}

const std::vector<unsigned>&
InfluenceManager::getAreas(std::span<const float> result, ParentBuildingType type, unsigned char player) const {
	if (type == ParentBuildingType::RESOURCE) {
		std::array<InfluenceMap*, RESOURCES_SIZE> maps = mapsGatherSpeedPerPlayer[player];
		return getBestVisibleIndexes(maps, result, player);
	}
	std::array<InfluenceMap*, AI_MAP_COUNT> maps = mapsForAiPerPlayer[player];
	return getBestVisibleIndexes(maps, result, player);
}

std::vector<unsigned> InfluenceManager::getAreasResBonus(unsigned char id, unsigned char player) const {
	return mapsResNotInBonusPerPlayer[player][id]->getRawMaxIdxs();
}

const std::vector<unsigned>&
InfluenceManager::getBestVisibleIndexes(std::span<InfluenceMap*> maps, std::span<const float> result, unsigned char player) const {
	assert(result.size() == maps.size());

	//unseen means max float max error
	const int noOfVisible = visibilityManager->removeUnseen(player, errorsSum);

	char numberOfNotEmptyMap = 0;
	for (auto&& [map, value] : std::views::zip(maps, result)) {
		numberOfNotEmptyMap += map->cumulateErrors(value, errorsSum);
	}

	const int size = Urho3D::Min(noOfVisible, arraySize / 8);

	collectSortedBelow(tempIndexes, std::span(errorsSum, arraySize), 0.1f * numberOfNotEmptyMap, size);
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

	assert(gatherSpeed[cast(ResourceType::FOOD)][playerId]->getResolution() == calculator->getResolution());
	const auto index = getIndexInInfluence(unit);
	gatherSpeed[resId][playerId]->update(index, value);

	econQuad[playerId]->update(index, value);
}

void InfluenceManager::addAttack(unsigned char player, const Urho3D::Vector2& position, float value) const {
	attackSpeed[player]->update(position, value);
}

std::optional<Urho3D::Vector2> InfluenceManager::getCenterOf(CenterType id, unsigned char player) const {
	return mapsForCentersPerPlayer[player][castC(id)]->getCenter();
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

const std::vector<Urho3D::Vector2>&
InfluenceManager::centersFromIndexes(const std::vector<unsigned>& indexes) const {
	tempCenters.clear();
	tempCenters.reserve(indexes.size());
	for (const auto value : indexes) {
		tempCenters.emplace_back(calculator->getCenter(value));
	}
	return tempCenters;
}

float InfluenceManager::getFieldSize() const {
	return calculator->getFieldSize();
}
