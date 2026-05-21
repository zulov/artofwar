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
	using I = BuildingInputIdx;
	constexpr auto e = [](I v) { return static_cast<int>(v); };

	auto* res = player->getResources();
	auto* possession = player->getPossession();

	// Building saturation (1)
	inputData[e(I::BUILDINGS_COUNT)] = static_cast<float>(possession->getBuildingsNumber()) / 50.f;

	// Workers — farm vs barracks decision (2)
	inputData[e(I::WORKERS_COUNT)] = static_cast<float>(possession->getWorkersNumber()) / 100.f;
	inputData[e(I::FREE_WORKERS)] = static_cast<float>(possession->getFreeWorkersNumber()) / 100.f;

	// Army — barracks vs defence decision (2)
	inputData[e(I::ARMY_COUNT)] = static_cast<float>(possession->getArmyNumber()) / 200.f;
	inputData[e(I::ATTACK_SUM)] = possession->getAttackSum() / 1000.f;

	// Defence — already have defence? (1)
	inputData[e(I::DEFENCE_ATTACK_SUM)] = possession->getDefenceAttackSum() / 100.f;

	// Gather speeds — which resource income is weak? (4)
	inputData[e(I::GATHER_FOOD)] = res->getGatherSpeed(ResourceType::FOOD) / 10.f;
	inputData[e(I::GATHER_WOOD)] = res->getGatherSpeed(ResourceType::WOOD) / 10.f;
	inputData[e(I::GATHER_STONE)] = res->getGatherSpeed(ResourceType::STONE) / 10.f;
	inputData[e(I::GATHER_GOLD)] = res->getGatherSpeed(ResourceType::GOLD) / 10.f;

	// Enemy threat — walls/towers decision (1)
	inputData[e(I::ENEMY_THREAT_DIST)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);

	// Urgencies from MasterBrain (3)
	inputData[e(I::BUILDING_URGENCY)] = buildingUrgency;
	inputData[e(I::TECH_URGENCY)] = techUrgency;
	inputData[e(I::DEFENCE_BUILDING_URGENCY)] = defenceBuildingUrgency;

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	return BuildingOutput{
		result[0], result[1], result[2], result[3], result[4],
		result[5], result[6], result[7], result[8], result[9], result[10]
	};
}
