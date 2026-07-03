#include "EconomyBrain.h"

#include <magic_enum.hpp>
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "AiHistory.h"
#include "AiUtils.h"
#include "NormScale.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"
#include "env/influence/CenterType.h"
#include "Game.h"
#include "env/Environment.h"
#include "env/EnvConsts.h"
#include "objects/resource/ResourceEntity.h"
#include "utils/OtherUtils.h"

#include <algorithm>
#include <cmath>

namespace {
std::pair<float, float> getNearbySupply(char playerId) {
	// Resources within the level-1 query radius of the economy center.
	constexpr ResourceQueryLevel SEARCH_LEVEL = ResourceQueryLevel::R128;

	auto* env = Game::getEnvironment();
	auto center = env->getCenterOf(CenterType::ECON, playerId);
	if (center.has_value()) {
		return env->sumFoodWoodWithin(center.value(), castC(SEARCH_LEVEL));
	}

	return {0.f, 0.f};
}
}

EconomyBrain::EconomyBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->brainPrefix[1] + "economy.csv")),
	  nation(nation) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<EconomyInputIdx>());
	assert(brain->getOutputSize() == magic_enum::enum_count<EconomyOutputIdx>());
}

EconomyOutput EconomyBrain::decide(Player* player, Player* enemy,
                                    const std::array<float, 4>& lackingPerResource,
                                    float economyUrgency, float workerUrgency, float expandUrgency,
                                    float techUrgency, float gameTime,
                                    const AiHistory* history) {
	using I = EconomyInputIdx;

	auto* res = player->getResources();
	auto* possession = player->getPossession();

	// Scores (2)
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), NormScale::SCORE);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), NormScale::SCORE);

	// Resource values (4)
	inputData[idx(I::RES_FOOD)] = norm(res->getValue(ResourceType::FOOD), NormScale::RES);
	inputData[idx(I::RES_WOOD)] = norm(res->getValue(ResourceType::WOOD), NormScale::RES);
	inputData[idx(I::RES_STONE)] = norm(res->getValue(ResourceType::STONE), NormScale::RES);
	inputData[idx(I::RES_GOLD)] = norm(res->getValue(ResourceType::GOLD), NormScale::RES);

	// Gather speeds (4)
	inputData[idx(I::GATHER_FOOD)] = norm(res->getGatherSpeed(ResourceType::FOOD), NormScale::GATHER);
	inputData[idx(I::GATHER_WOOD)] = norm(res->getGatherSpeed(ResourceType::WOOD), NormScale::GATHER);
	inputData[idx(I::GATHER_STONE)] = norm(res->getGatherSpeed(ResourceType::STONE), NormScale::GATHER);
	inputData[idx(I::GATHER_GOLD)] = norm(res->getGatherSpeed(ResourceType::GOLD), NormScale::GATHER);

	// Workers (2)
	inputData[idx(I::FREE_WORKERS)] = norm(possession->getFreeWorkersNumber(), NormScale::WORKERS);
	inputData[idx(I::WORKERS_COUNT)] = norm(possession->getWorkersNumber(), NormScale::WORKERS);

	// Storage ratios (2) — from Resources
	float foodStorage = res->getFoodStorage();
	inputData[idx(I::FOOD_STORAGE_RATIO)] = foodStorage > 0.f ? norm(res->getValue(ResourceType::FOOD), foodStorage) : 0.f;
	float goldStorage = res->getGoldStorage();
	inputData[idx(I::GOLD_STORAGE_RATIO)] = goldStorage > 0.f ? norm(res->getValue(ResourceType::GOLD), goldStorage) : 0.f;

	// Refine ratios (2) — from Resources
	float stoneGather = res->getGatherSpeed(ResourceType::STONE);
	float stoneCap = res->getStoneRefineCapacity();
	inputData[idx(I::STONE_REFINE_RATIO)] = stoneCap > 0.f ? norm(stoneGather, stoneCap) : 0.f;
	float goldGather = res->getGatherSpeed(ResourceType::GOLD);
	float goldCap = res->getGoldRefineCapacity();
	inputData[idx(I::GOLD_REFINE_RATIO)] = goldCap > 0.f ? norm(goldGather, goldCap) : 0.f;

	// Refine gaps (2) — from Resources
	inputData[idx(I::STONE_REFINE_GAP)] = norm(std::max(0.f, stoneGather - stoneCap), NormScale::REFINE_GAP);
	inputData[idx(I::GOLD_REFINE_GAP)] = norm(std::max(0.f, goldGather - goldCap), NormScale::REFINE_GAP);

	// Bonus coverage (4) — from Possession
	inputData[idx(I::BONUS_COVERAGE_FOOD)] = possession->getBonusCoverage(ResourceType::FOOD);
	inputData[idx(I::BONUS_COVERAGE_WOOD)] = possession->getBonusCoverage(ResourceType::WOOD);
	inputData[idx(I::BONUS_COVERAGE_STONE)] = possession->getBonusCoverage(ResourceType::STONE);
	inputData[idx(I::BONUS_COVERAGE_GOLD)] = possession->getBonusCoverage(ResourceType::GOLD);

	// Nearby resource supply (2)
	auto [foodSupply, woodSupply] = getNearbySupply(player->getId());
	inputData[idx(I::NEARBY_FOOD_SUPPLY)] = norm(foodSupply, NormScale::NEARBY_SUPPLY);
	inputData[idx(I::NEARBY_WOOD_SUPPLY)] = norm(woodSupply, NormScale::NEARBY_SUPPLY);
	inputData[idx(I::TOTAL_RES_BUILDINGS)] = norm(possession->getResourceBuildingCount(), NormScale::RES_BUILDINGS);
	inputData[idx(I::RES_WO_BONUS)] = norm(possession->getResWithoutBonusSum(), NormScale::RES_BUILDINGS);

	// Lacking per resource (4)
	inputData[idx(I::LACKING_FOOD)] = norm(lackingPerResource[0], NormScale::LACKING_PER_RES);
	inputData[idx(I::LACKING_WOOD)] = norm(lackingPerResource[1], NormScale::LACKING_PER_RES);
	inputData[idx(I::LACKING_STONE)] = norm(lackingPerResource[2], NormScale::LACKING_PER_RES);
	inputData[idx(I::LACKING_GOLD)] = norm(lackingPerResource[3], NormScale::LACKING_PER_RES);

	// Urgencies from MasterBrain (3)
	inputData[idx(I::ECONOMY_URGENCY)] = economyUrgency;
	inputData[idx(I::WORKER_URGENCY)] = workerUrgency;
	inputData[idx(I::EXPAND_URGENCY)] = expandUrgency;

	// History — collection failures signal resource scarcity nearby
	constexpr unsigned int LOOKBACK = 1800;
	inputData[idx(I::RECENT_COLLECT_FAILURES)] = norm(history->collectFailureScore(LOOKBACK), NormScale::BUILD_FAILURE);

	// Upgrade inputs (4)
	inputData[idx(I::TECH_URGENCY)] = techUrgency;

	inputData[idx(I::AVG_WORKER_LEVEL)] = avgUnitLevel(nation->workers, player,
		[](const db_unit*) { return true; });

	inputData[idx(I::AVG_RES_BUILDING_LEVEL)] = avgBuildingLevel(nation->buildings, player, ParentBuildingType::RESOURCE);

	inputData[idx(I::GAME_TIME)] = gameTime;

	auto result = brain->decide(inputData);

	using O = EconomyOutputIdx;

	float workerAlloc = result[idx(O::WORKER_ALLOCATION)];
	unsigned char workerCount = 0;
	if (workerAlloc > 0.1f) {
		int raw = roundToInt(workerAlloc * MAX_WORKERS_PER_TICK);
		workerCount = static_cast<unsigned char>(std::max(1, raw));
	}

	return EconomyOutput{
		workerAlloc,
		result[idx(O::FOOD_PRIORITY)],
		result[idx(O::WOOD_PRIORITY)],
		result[idx(O::STONE_PRIORITY)],
		result[idx(O::GOLD_PRIORITY)],
		result[idx(O::EXPAND_PRIORITY)],
		result[idx(O::REASSIGN_WORKERS)],
		result[idx(O::NEED_BONUS_FOOD)],
		result[idx(O::NEED_BONUS_WOOD)],
		result[idx(O::NEED_BONUS_STONE)],
		result[idx(O::NEED_BONUS_GOLD)],
		result[idx(O::NEED_FOOD_SOURCE)],
		result[idx(O::NEED_FOOD_STORAGE)],
		result[idx(O::NEED_GOLD_STORAGE)],
		result[idx(O::NEED_GOLD_REFINE)],
		result[idx(O::NEED_STONE_REFINE)],
		result[idx(O::NEED_WOOD_SOURCE)],
		result[idx(O::WORKER_UPGRADE_URGENCY)],
		result[idx(O::RES_BUILDING_UPGRADE_URGENCY)],
		workerCount
	};
}
