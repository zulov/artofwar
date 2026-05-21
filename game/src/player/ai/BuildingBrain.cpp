#include "BuildingBrain.h"

#include <magic_enum.hpp>
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "MetricDefinitions.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"
#include "env/influence/CenterType.h"

BuildingBrain::BuildingBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->actionPrefix[2] + "building.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getOutputSize() == magic_enum::enum_count<BuildingOutputIdx>());
}

BuildingOutput BuildingBrain::decide(Player* player, Player* enemy,
                                      float buildingUrgency, float techUrgency, float defenceBuildingUrgency) {
	// 14 inputs — no resource stockpiles (want list handles affordability)
	int idx = 0;

	auto* res = player->getResources();
	auto* possession = player->getPossession();

	// Building saturation (1)
	inputData[idx++] = static_cast<float>(possession->getBuildingsNumber()) / 50.f;

	// Workers — farm vs barracks decision (2)
	inputData[idx++] = static_cast<float>(possession->getWorkersNumber()) / 100.f;
	inputData[idx++] = static_cast<float>(possession->getFreeWorkersNumber()) / 100.f;

	// Army — barracks vs defence decision (2)
	inputData[idx++] = static_cast<float>(possession->getArmyNumber()) / 200.f;
	inputData[idx++] = possession->getAttackSum() / 1000.f;

	// Defence — already have defence? (1)
	inputData[idx++] = possession->getDefenceAttackSum() / 100.f;

	// Gather speeds — which resource income is weak? (4)
	inputData[idx++] = res->getGatherSpeed(ResourceType::FOOD) / 10.f;
	inputData[idx++] = res->getGatherSpeed(ResourceType::WOOD) / 10.f;
	inputData[idx++] = res->getGatherSpeed(ResourceType::STONE) / 10.f;
	inputData[idx++] = res->getGatherSpeed(ResourceType::GOLD) / 10.f;

	// Enemy threat — walls/towers decision (1)
	inputData[idx++] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);

	// Urgencies from MasterBrain (3)
	inputData[idx++] = buildingUrgency;
	inputData[idx++] = techUrgency;
	inputData[idx++] = defenceBuildingUrgency;

	assert(idx == 14);

	auto result = brain->decide(std::span<const float>(inputData.data(), 14));

	return BuildingOutput{
		result[0], result[1], result[2], result[3], result[4],
		result[5], result[6], result[7], result[8], result[9], result[10]
	};
}
