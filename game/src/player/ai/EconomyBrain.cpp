#include "EconomyBrain.h"

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
}

EconomyOutput EconomyBrain::decide(Player* player, Player* enemy,
                                    std::span<const float> lackingPerResource,
                                    float economyUrgency, float workerUrgency, float expandUrgency) {
	std::array<float, 20> inputData{};
	int idx = 0;

	auto* res = player->getResources();
	auto* possession = player->getPossession();

	// Resource values (4)
	inputData[idx++] = res->getValue(ResourceType::FOOD) / 1000.f;
	inputData[idx++] = res->getValue(ResourceType::WOOD) / 1000.f;
	inputData[idx++] = res->getValue(ResourceType::STONE) / 1000.f;
	inputData[idx++] = res->getValue(ResourceType::GOLD) / 1000.f;

	// Gather speeds (4)
	inputData[idx++] = res->getGatherSpeed(ResourceType::FOOD) / 10.f;
	inputData[idx++] = res->getGatherSpeed(ResourceType::WOOD) / 10.f;
	inputData[idx++] = res->getGatherSpeed(ResourceType::STONE) / 10.f;
	inputData[idx++] = res->getGatherSpeed(ResourceType::GOLD) / 10.f;

	// Workers (2)
	inputData[idx++] = static_cast<float>(possession->getFreeWorkersNumber()) / 100.f;
	inputData[idx++] = static_cast<float>(possession->getWorkersNumber()) / 100.f;

	// Army count — economy must balance military needs (1)
	inputData[idx++] = static_cast<float>(possession->getArmyNumber()) / 200.f;

	// Enemy threat proximity — should we play safe with economy? (1)
	inputData[idx++] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);

	// Enemy score — overall enemy strength (1)
	inputData[idx++] = enemy->getScore() / 1000.f;

	// Lacking per resource (4)
	for (int i = 0; i < 4; ++i) {
		inputData[idx++] = lackingPerResource.size() > static_cast<size_t>(i) ? lackingPerResource[i] / 500.f : 0.f;
	}

	// Urgencies from MasterBrain (3)
	inputData[idx++] = economyUrgency;
	inputData[idx++] = workerUrgency;
	inputData[idx++] = expandUrgency;

	assert(idx == 20);

	auto result = brain->decide(std::span<const float>(inputData.data(), 20));

	return EconomyOutput{
		result[0], result[1], result[2], result[3],
		result[4], result[5]
	};
}
