#include "EconomyBrain.h"

#include <magic_enum.hpp>
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "AiHistory.h"
#include "AiUtils.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"
#include "env/influence/CenterType.h"
#include "Game.h"
#include "env/Environment.h"
#include "objects/resource/ResourceEntity.h"

#include <algorithm>
#include <cmath>

namespace {
std::pair<float, float> getNearbySupply(char playerId) {
	constexpr float SEARCH_RADIUS = 128.f;
	float foodHp = 0.f, woodHp = 0.f;

	auto* env = Game::getEnvironment();
	auto center = env->getCenterOf(CenterType::ECON, playerId);
	if (center.has_value()) {
		auto* resources = env->getResources(center.value(), SEARCH_RADIUS);
		for (auto* phys : *resources) {
			auto* resEntity = static_cast<ResourceEntity*>(phys);
			char resId = resEntity->getResourceId();
			if (resId == 0) {
				foodHp += resEntity->getHp();
			} else if (resId == 1) {
				woodHp += resEntity->getHp();
			}
		}
	}

	return {foodHp, woodHp};
}
}

EconomyBrain::EconomyBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->actionPrefix[1] + "economy.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<EconomyInputIdx>());
	assert(brain->getOutputSize() == magic_enum::enum_count<EconomyOutputIdx>());
}

EconomyOutput EconomyBrain::decide(Player* player, Player* enemy,
                                    const std::array<float, 4>& lackingPerResource,
                                    float economyUrgency, float workerUrgency, float expandUrgency,
                                    const AiHistory* history) {
	using I = EconomyInputIdx;

	auto* res = player->getResources();
	auto* possession = player->getPossession();

	// Scores (2)
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), 1000.f);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), 1000.f);

	// Resource values (4)
	inputData[idx(I::RES_FOOD)] = norm(res->getValue(ResourceType::FOOD), 1000.f);
	inputData[idx(I::RES_WOOD)] = norm(res->getValue(ResourceType::WOOD), 1000.f);
	inputData[idx(I::RES_STONE)] = norm(res->getValue(ResourceType::STONE), 1000.f);
	inputData[idx(I::RES_GOLD)] = norm(res->getValue(ResourceType::GOLD), 1000.f);

	// Gather speeds (4)
	inputData[idx(I::GATHER_FOOD)] = norm(res->getGatherSpeed(ResourceType::FOOD), 10.f);
	inputData[idx(I::GATHER_WOOD)] = norm(res->getGatherSpeed(ResourceType::WOOD), 10.f);
	inputData[idx(I::GATHER_STONE)] = norm(res->getGatherSpeed(ResourceType::STONE), 10.f);
	inputData[idx(I::GATHER_GOLD)] = norm(res->getGatherSpeed(ResourceType::GOLD), 10.f);

	// Workers (2)
	inputData[idx(I::FREE_WORKERS)] = norm(possession->getFreeWorkersNumber(), 100.f);
	inputData[idx(I::WORKERS_COUNT)] = norm(possession->getWorkersNumber(), 100.f);

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
	inputData[idx(I::STONE_REFINE_GAP)] = norm(std::max(0.f, stoneGather - stoneCap), 10.f);
	inputData[idx(I::GOLD_REFINE_GAP)] = norm(std::max(0.f, goldGather - goldCap), 10.f);

	// Bonus coverage (4) — from Possession
	inputData[idx(I::BONUS_COVERAGE_FOOD)] = possession->getBonusCoverage(ResourceType::FOOD);
	inputData[idx(I::BONUS_COVERAGE_WOOD)] = possession->getBonusCoverage(ResourceType::WOOD);
	inputData[idx(I::BONUS_COVERAGE_STONE)] = possession->getBonusCoverage(ResourceType::STONE);
	inputData[idx(I::BONUS_COVERAGE_GOLD)] = possession->getBonusCoverage(ResourceType::GOLD);

	// Nearby resource supply (2)
	auto [foodSupply, woodSupply] = getNearbySupply(player->getId());
	inputData[idx(I::NEARBY_FOOD_SUPPLY)] = norm(foodSupply, 5000.f);
	inputData[idx(I::NEARBY_WOOD_SUPPLY)] = norm(woodSupply, 5000.f);
	inputData[idx(I::TOTAL_RES_BUILDINGS)] = norm(possession->getResourceBuildingCount(), 20.f);
	inputData[idx(I::RES_WO_BONUS)] = norm(possession->getResWithoutBonusSum(), 20.f);

	// Lacking per resource (4)
	inputData[idx(I::LACKING_FOOD)] = norm(lackingPerResource[0], 500.f);
	inputData[idx(I::LACKING_WOOD)] = norm(lackingPerResource[1], 500.f);
	inputData[idx(I::LACKING_STONE)] = norm(lackingPerResource[2], 500.f);
	inputData[idx(I::LACKING_GOLD)] = norm(lackingPerResource[3], 500.f);

	// Urgencies from MasterBrain (3)
	inputData[idx(I::ECONOMY_URGENCY)] = economyUrgency;
	inputData[idx(I::WORKER_URGENCY)] = workerUrgency;
	inputData[idx(I::EXPAND_URGENCY)] = expandUrgency;

	// History — collection failures signal resource scarcity nearby
	constexpr unsigned int LOOKBACK = 1800;
	inputData[idx(I::RECENT_COLLECT_FAILURES)] = norm(history->collectFailureScore(LOOKBACK), 10.f);

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	using O = EconomyOutputIdx;
	constexpr auto o = [](O v) { return static_cast<int>(v); };

	float workerAlloc = result[o(O::WORKER_ALLOCATION)];
	unsigned char workerCount = 0;
	if (workerAlloc > 0.1f) {
		int raw = static_cast<int>(std::round(workerAlloc * MAX_WORKERS_PER_TICK));
		workerCount = static_cast<unsigned char>(std::max(1, raw));
	}

	return EconomyOutput{
		workerAlloc,
		workerCount,
		result[o(O::FOOD_PRIORITY)],
		result[o(O::WOOD_PRIORITY)],
		result[o(O::STONE_PRIORITY)],
		result[o(O::GOLD_PRIORITY)],
		result[o(O::EXPAND_PRIORITY)],
		result[o(O::REASSIGN_WORKERS)],
		result[o(O::NEED_BONUS_FOOD)],
		result[o(O::NEED_BONUS_WOOD)],
		result[o(O::NEED_BONUS_STONE)],
		result[o(O::NEED_BONUS_GOLD)],
		result[o(O::NEED_FOOD_SOURCE)],
		result[o(O::NEED_FOOD_STORAGE)],
		result[o(O::NEED_GOLD_STORAGE)],
		result[o(O::NEED_GOLD_REFINE)],
		result[o(O::NEED_STONE_REFINE)],
		result[o(O::NEED_WOOD_SOURCE)]
	};
}
