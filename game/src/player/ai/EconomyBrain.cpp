#include "EconomyBrain.h"

#include <magic_enum.hpp>
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "MetricDefinitions.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"
#include "env/influence/CenterType.h"

EconomyBrain::EconomyBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->actionPrefix[1] + "economy.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<EconomyInputIdx>());
	assert(brain->getOutputSize() == magic_enum::enum_count<EconomyOutputIdx>());
}

EconomyOutput EconomyBrain::decide(Player* player, Player* enemy,
                                    const std::array<float, 4>& lackingPerResource,
                                    float economyUrgency, float workerUrgency, float expandUrgency) {
	using I = EconomyInputIdx;
	constexpr auto e = [](I v) { return static_cast<int>(v); };

	auto* res = player->getResources();
	auto* possession = player->getPossession();

	// Scores (2)
	inputData[e(I::PLAYER_SCORE)] = player->getScore() / 1000.f;
	inputData[e(I::ENEMY_SCORE)] = enemy->getScore() / 1000.f;

	// Resource values (4)
	inputData[e(I::RES_FOOD)] = res->getValue(ResourceType::FOOD) / 1000.f;
	inputData[e(I::RES_WOOD)] = res->getValue(ResourceType::WOOD) / 1000.f;
	inputData[e(I::RES_STONE)] = res->getValue(ResourceType::STONE) / 1000.f;
	inputData[e(I::RES_GOLD)] = res->getValue(ResourceType::GOLD) / 1000.f;

	// Gather speeds (4)
	inputData[e(I::GATHER_FOOD)] = res->getGatherSpeed(ResourceType::FOOD) / 10.f;
	inputData[e(I::GATHER_WOOD)] = res->getGatherSpeed(ResourceType::WOOD) / 10.f;
	inputData[e(I::GATHER_STONE)] = res->getGatherSpeed(ResourceType::STONE) / 10.f;
	inputData[e(I::GATHER_GOLD)] = res->getGatherSpeed(ResourceType::GOLD) / 10.f;

	// Workers (2)
	inputData[e(I::FREE_WORKERS)] = static_cast<float>(possession->getFreeWorkersNumber()) / 100.f;
	inputData[e(I::WORKERS_COUNT)] = static_cast<float>(possession->getWorkersNumber()) / 100.f;

	// New inputs — TODO implement
	inputData[e(I::FOOD_STORAGE_RATIO)] = 0.f; //TODO implement
	inputData[e(I::GOLD_STORAGE_RATIO)] = 0.f; //TODO implement
	inputData[e(I::STONE_REFINE_RATIO)] = 0.f; //TODO implement
	inputData[e(I::GOLD_REFINE_RATIO)] = 0.f; //TODO implement
	inputData[e(I::STONE_REFINE_GAP)] = 0.f; //TODO res->getStoneRefineGap() / 10.f
	inputData[e(I::GOLD_REFINE_GAP)] = 0.f; //TODO res->getGoldRefineGap() / 10.f
	inputData[e(I::BONUS_COVERAGE_FOOD)] = 0.f; //TODO possession->getBonusCoverage(FOOD)
	inputData[e(I::BONUS_COVERAGE_WOOD)] = 0.f; //TODO possession->getBonusCoverage(WOOD)
	inputData[e(I::BONUS_COVERAGE_STONE)] = 0.f; //TODO possession->getBonusCoverage(STONE)
	inputData[e(I::BONUS_COVERAGE_GOLD)] = 0.f; //TODO possession->getBonusCoverage(GOLD)
	inputData[e(I::FARM_COUNT)] = 0.f; //TODO possession->getConvertBuildingCount() / 5.f
	inputData[e(I::SPAWNER_COUNT)] = 0.f; //TODO possession->getSpawnerCount() / 5.f
	inputData[e(I::FOOD_DECAY_RATE)] = 0.f; //TODO res->getFoodDecayRate() / 100.f
	inputData[e(I::TOTAL_RES_BUILDINGS)] = 0.f; //TODO possession->getResourceBuildingCount() / 20.f
	inputData[e(I::RES_WO_BONUS)] = 0.f; //TODO implement

	// Lacking per resource (4)
	inputData[e(I::LACKING_FOOD)] = lackingPerResource[0] / 500.f;
	inputData[e(I::LACKING_WOOD)] = lackingPerResource[1] / 500.f;
	inputData[e(I::LACKING_STONE)] = lackingPerResource[2] / 500.f;
	inputData[e(I::LACKING_GOLD)] = lackingPerResource[3] / 500.f;

	// Urgencies from MasterBrain (3)
	inputData[e(I::ECONOMY_URGENCY)] = economyUrgency;
	inputData[e(I::WORKER_URGENCY)] = workerUrgency;
	inputData[e(I::EXPAND_URGENCY)] = expandUrgency;

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	using O = EconomyOutputIdx;
	constexpr auto o = [](O v) { return static_cast<int>(v); };

	return EconomyOutput{
		result[o(O::WORKER_ALLOCATION)],
		result[o(O::FOOD_PRIORITY)],
		result[o(O::WOOD_PRIORITY)],
		result[o(O::STONE_PRIORITY)],
		result[o(O::GOLD_PRIORITY)],
		result[o(O::EXPAND_PRIORITY)],
		result[o(O::RESOURCE_BUILDING_URGENCY)],
		result[o(O::REASSIGN_WORKERS)],
		result[o(O::NEED_MILL)],
		result[o(O::NEED_SAWMILL)],
		result[o(O::NEED_MINE_S)],
		result[o(O::NEED_MINE_G)],
		result[o(O::NEED_FARM)],
		result[o(O::NEED_GRANARY)],
		result[o(O::NEED_BANK)],
		result[o(O::NEED_GOLD_REFINERY)],
		result[o(O::NEED_STONE_REFINERY)],
		result[o(O::NEED_TREE_NURSERY)]
	};
}
