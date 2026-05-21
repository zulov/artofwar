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
                                    std::span<const float> lackingPerResource,
                                    float economyUrgency, float workerUrgency, float expandUrgency) {
	using I = EconomyInputIdx;
	constexpr auto e = [](I v) { return static_cast<int>(v); };

	auto* res = player->getResources();
	auto* possession = player->getPossession();

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

	// Army count (1)
	inputData[e(I::ARMY_COUNT)] = static_cast<float>(possession->getArmyNumber()) / 200.f;

	// Enemy threat proximity (1)
	inputData[e(I::ENEMY_THREAT_DIST)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);

	// Enemy score (1)
	inputData[e(I::ENEMY_SCORE)] = enemy->getScore() / 1000.f;

	// Lacking per resource (4)
	inputData[e(I::LACKING_FOOD)] = lackingPerResource.size() > 0 ? lackingPerResource[0] / 500.f : 0.f;
	inputData[e(I::LACKING_WOOD)] = lackingPerResource.size() > 1 ? lackingPerResource[1] / 500.f : 0.f;
	inputData[e(I::LACKING_STONE)] = lackingPerResource.size() > 2 ? lackingPerResource[2] / 500.f : 0.f;
	inputData[e(I::LACKING_GOLD)] = lackingPerResource.size() > 3 ? lackingPerResource[3] / 500.f : 0.f;

	// Urgencies from MasterBrain (3)
	inputData[e(I::ECONOMY_URGENCY)] = economyUrgency;
	inputData[e(I::WORKER_URGENCY)] = workerUrgency;
	inputData[e(I::EXPAND_URGENCY)] = expandUrgency;

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	return EconomyOutput{
		result[0], result[1], result[2], result[3],
		result[4], result[5]
	};
}
