#include "InfluenceManager.h"
#include <exprtk/exprtk.hpp>
#include <magic_enum.hpp>
#include "CenterType.h"
#include "MapsUtils.h"
#include "VisibilityManager.h"
#include "map/InfluenceMapHistory.h"
#include "map/InfluenceMapQuad.h"
#include "debug/DebugLineRepo.h"
#include "map/InfluenceMapInt.h"
#include "math/VectorUtils.h"
#include "env/bucket/CellEnums.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "env/GridCalculatorProvider.h"
#include "env/ContentInfo.h"
#include "utils/AssertUtils.h"
#include "utils/OtherUtils.h"

// mapsForCentersPerPlayer is sized CENTER_TYPE_COUNT and indexed by CenterType.
static_assert(magic_enum::enum_count<CenterType>() == CENTER_TYPE_COUNT,
              "CENTER_TYPE_COUNT must equal the number of CenterType enumerators");


InfluenceManager::InfluenceManager(unsigned char numberOfPlayers, float mapSize, Urho3D::Terrain* terrain) {
	unitsNumberPerPlayer.reserve(numberOfPlayers);
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
	buildingsQuad.reserve(numberOfPlayers);
	econQuad.reserve(numberOfPlayers);

	mapsForAiArmyPerPlayer.reserve(numberOfPlayers);
	mapsForAiPerPlayer.reserve(numberOfPlayers);
	mapsForCentersPerPlayer.reserve(numberOfPlayers);
	unsigned short resolution = mapSize / INF_GRID_FIELD_SIZE;
	sharedTemplateV = InfluenceMapFloat::createTemplateV(0.5f, INF_LEVEL);
	for (int player = 0; player < numberOfPlayers; ++player) {
		unitsNumberPerPlayer.emplace_back(new InfluenceMapInt(resolution, mapSize, 40));
		buildingsInfluencePerPlayer.emplace_back(
		                                         new InfluenceMapFloat(resolution, mapSize, 0.5f, INF_LEVEL, 2, sharedTemplateV));
		unitsInfluencePerPlayer.emplace_back(
		                                     new InfluenceMapFloat(resolution, mapSize, 0.5f, INF_LEVEL, 40, sharedTemplateV));

		for (auto& gs : gatherSpeed) {
			gs.emplace_back(new InfluenceMapHistory(resolution, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40, sharedTemplateV));
		}

		for (auto& resNotInBonus : resNotInBonus) {
			resNotInBonus.emplace_back(new InfluenceMapInt(resolution, mapSize, 5));
		}

		attackSpeed.emplace_back(new InfluenceMapHistory(resolution, mapSize, 0.5f, INF_LEVEL, 0.0001f, 0.5f, 40, sharedTemplateV));
		armyQuad.emplace_back(new InfluenceMapQuad(resolution, mapSize));
		buildingsQuad.emplace_back(new InfluenceMapQuad(resolution, mapSize));
		econQuad.emplace_back(new InfluenceMapQuad(mapSize / INF_GRID_FIELD_SIZE, mapSize));
	}

	for (int player = 0; player < numberOfPlayers; ++player) {
		const int enemy = 1 - player;
		std::array<InfluenceMapFloat*, RESOURCES_SIZE> gatherSpeedView;
		std::array<InfluenceMapInt*, RESOURCES_SIZE> resNotInBonusView;
		for (int r = 0; r < RESOURCES_SIZE; ++r) {
			gatherSpeedView[r] = gatherSpeed[r][player];
			resNotInBonusView[r] = resNotInBonus[r][player];
		}

		mapsForAiPerPlayer.emplace_back(std::array<InfluenceMapFloat*, AI_MAP_COUNT>{
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
		mapsForAiArmyPerPlayer.emplace_back(std::array<InfluenceMapFloat*, AI_ARMY_MAP_COUNT>{
			                                    buildingsInfluencePerPlayer[player],
			                                    unitsInfluencePerPlayer[player],
			                                    attackSpeed[player],
			                                    buildingsInfluencePerPlayer[enemy],
			                                    unitsInfluencePerPlayer[enemy],
			                                    attackSpeed[enemy],
		                                    });
		mapsForCentersPerPlayer.emplace_back(std::array<InfluenceMapQuad*, CENTER_TYPE_COUNT>{
			                                     econQuad[player],
			                                     buildingsQuad[player],
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
	intersection = new float[arraySize];

	assert(unitsNumberPerPlayer[0]->getResolution() == unitsInfluencePerPlayer[0]->getResolution()
	       && calculator->getResolution() == unitsNumberPerPlayer[0]->getResolution());
}

InfluenceManager::~InfluenceManager() {
	clear_vector(unitsNumberPerPlayer);
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
	clear_vector(buildingsQuad);
	clear_vector(econQuad);

	delete visibilityManager;
	delete ci;

	delete[]intersection;
	delete[] sharedTemplateV;
}

void InfluenceManager::updateUnits(std::vector<Unit*>* units) const {
	MapsUtils::resetMaps(unitsInfluencePerPlayer);
	if (SIM_GLOBALS.HEADLESS) {
		for (const auto unit : (*units)) {
			const auto pId = unit->getPlayer();
			const auto index = getIndexInInfluence(unit);
			unitsInfluencePerPlayer[pId]->tempUpdate(index);
		}
	} else {
		MapsUtils::resetMaps(unitsNumberPerPlayer);
		for (const auto unit : (*units)) {
			const auto pId = unit->getPlayer();
			const auto index = getIndexInInfluence(unit);
			unitsNumberPerPlayer[pId]->updateInt(index);
			unitsInfluencePerPlayer[pId]->tempUpdate(index);
		}
	}

	MapsUtils::finalize(unitsInfluencePerPlayer);
}

void InfluenceManager::updateQuadUnits(const std::vector<Unit*>* units) const {
	MapsUtils::resetMaps(armyQuad);
	for (const auto unit : (*units)) {
		if (!unit->getDb()->typeWorker) {
			armyQuad[unit->getPlayer()]->updateInt(getIndexInInfluence(unit));
		}
	}
}

void InfluenceManager::updateBuildings(const std::vector<Building*>* buildings) const {
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
	for (auto& vec : gatherSpeed) {
		MapsUtils::resetMaps(vec);
		MapsUtils::finalize(vec);
	}

	MapsUtils::resetMaps(attackSpeed);
	MapsUtils::finalize(attackSpeed);
}

void InfluenceManager::updateNotInBonus(std::vector<Unit*>* units) const {
	for (auto& vec : resNotInBonus) {
		MapsUtils::resetMaps(vec);
	}
	//TODO perf tylko workerów sprawdzić
	for (const auto unit : *units) {
		if (unit->getDb()->typeWorker && unit->getState() == UnitState::COLLECT && unit->isFirstThingAlive()) {
			auto res = (ResourceEntity*)unit->getThingToInteract();
			//TODO albo uzyc occupied cell tylko trzeba jakos przeliczyc
			mapsResNotInBonusPerPlayer[unit->getPlayer()][res->getResourceId()]->updateInt(res->getIndexInInfluence());
		}
	}
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
		MapsUtils::drawMap(currentDebugBatch, index, unitsNumberPerPlayer);
		break;
	case InfluenceDataType::UNITS_INFLUENCE_PER_PLAYER:
		MapsUtils::drawMap(currentDebugBatch, index, unitsInfluencePerPlayer);
		break;
	case InfluenceDataType::BUILDING_INFLUENCE_PER_PLAYER:
		MapsUtils::drawMap(currentDebugBatch, index, buildingsInfluencePerPlayer);
		break;
	case InfluenceDataType::FOOD_SPEED:
		MapsUtils::drawMap(currentDebugBatch, index, gatherSpeed[cast(ResourceType::FOOD)]);
		break;
	case InfluenceDataType::WOOD_SPEED:
		MapsUtils::drawMap(currentDebugBatch, index, gatherSpeed[cast(ResourceType::WOOD)]);
		break;
	case InfluenceDataType::STONE_SPEED:
		MapsUtils::drawMap(currentDebugBatch, index, gatherSpeed[cast(ResourceType::STONE)]);
		break;
	case InfluenceDataType::GOLD_SPEED:
		MapsUtils::drawMap(currentDebugBatch, index, gatherSpeed[cast(ResourceType::GOLD)]);
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

	constexpr const char* gatherNames[] = {"gatherFood", "gatherWood", "gatherStone", "gatherGold"};
	for (int r = 0; r < RESOURCES_SIZE; ++r) {
		MapsUtils::drawAll(gatherSpeed[r], gatherNames[r]);
	}
	MapsUtils::drawAll(attackSpeed, "attack");

	MapsUtils::drawAll(armyQuad, "armyQuad");
	MapsUtils::drawAll(buildingsQuad, "buildingsQuad");
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



std::vector<Urho3D::Vector2> InfluenceManager::getAreasIterative(std::span<const float> result, unsigned char player) {
	//TODO better!!!
	if (result.size() == AI_MAP_COUNT) {
		return centersFromIndexes(getAreas(mapsForAiPerPlayer[player] ,result, player));
	}
	if (result.size() == AI_ARMY_MAP_COUNT) {
		return centersFromIndexes(getAreas(mapsForAiArmyPerPlayer[player], result, player));
	}
	assert(false);
}

const std::vector<unsigned>&
InfluenceManager::getAreas(std::span<const float> result, ParentBuildingType type, unsigned char player) const {
	if (type == ParentBuildingType::RESOURCE) {
		std::array<InfluenceMapFloat*, RESOURCES_SIZE> maps = mapsGatherSpeedPerPlayer[player];
		return getAreas(maps, result, player);
	}
	std::array<InfluenceMapFloat*, AI_MAP_COUNT> maps = mapsForAiPerPlayer[player];
	return getAreas(maps, result, player);
}

std::vector<unsigned> InfluenceManager::getAreasResBonus(unsigned char id, unsigned char player) const {
	return mapsResNotInBonusPerPlayer[player][id]->getMaxIdxs();
}

const std::vector<unsigned>&
InfluenceManager::getAreas(std::span<InfluenceMapFloat*> maps, std::span<const float> result, unsigned char player) const {
	assert(result.size() == maps.size());

	const int noOfVisible = visibilityManager->removeUnseen(player, intersection);

	char numberOfNotEmptyMap = 0;
	for (char i = 0; i < maps.size(); ++i) {
		const auto map = maps[i];
		map->ensureReady();
		const auto ok = map->cumulateErros(result[i], intersection);
		numberOfNotEmptyMap += ok;
	}

	const int size = Urho3D::Min(noOfVisible, calculator->getResolution() * calculator->getResolution() / 8);

	collectSortedBelow(tempIndexes, std::span(intersection, arraySize), 0.1f * numberOfNotEmptyMap, size);
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
	gatherSpeed[resId][playerId]->tempUpdate(index, value);

	econQuad[playerId]->update(index, value);
}

void InfluenceManager::addAttack(unsigned char player, const Urho3D::Vector2& position, float value) const {
	attackSpeed[player]->tempUpdate(position, value);
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

std::vector<Urho3D::Vector2> InfluenceManager::centersFromIndexes(const std::vector<unsigned>& indexes) const {
	std::vector<Urho3D::Vector2> centers;
	centers.reserve(indexes.size());
	for (const auto value : indexes) {
		centers.emplace_back(calculator->getCenter(value));
	}
	return centers;
}

float InfluenceManager::getFieldSize() const {
	return unitsInfluencePerPlayer[0]->getFieldSize();
}
