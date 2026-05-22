#include "BuildSpatialBrain.h"

#include <magic_enum.hpp>
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"

BuildSpatialBrain::BuildSpatialBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->actionPrefix[0] + "build_spatial.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<BuildSpatialInputIdx>());
	assert(brain->getOutputSize() == magic_enum::enum_count<BuildSpatialOutputIdx>());
}

BuildSpatialOutput BuildSpatialBrain::decide(Player* player, Player* enemy,
                                              float buildingUrgency, float expandUrgency, float defenceBuildingUrgency) {
	using I = BuildSpatialInputIdx;
	constexpr auto e = [](I v) { return static_cast<int>(v); };

	auto* possession = player->getPossession();
	auto* res = player->getResources();

	// Urgencies from MasterBrain (3)
	inputData[e(I::BUILDING_URGENCY)] = buildingUrgency;
	inputData[e(I::EXPAND_URGENCY)] = expandUrgency;
	inputData[e(I::DEFENCE_BUILDING_URGENCY)] = defenceBuildingUrgency;

	// Counts (3)
	inputData[e(I::BUILDINGS_COUNT)] = static_cast<float>(possession->getBuildingsNumber()) / 50.f;
	inputData[e(I::WORKERS_COUNT)] = static_cast<float>(possession->getWorkersNumber()) / 100.f;
	inputData[e(I::ARMY_COUNT)] = static_cast<float>(possession->getArmyNumber()) / 200.f;

	// Game state (3)
	inputData[e(I::GAME_TIME)] = 0.f; //TODO implement
	inputData[e(I::PLAYER_SCORE)] = player->getScore() / 1000.f;
	inputData[e(I::ENEMY_SCORE)] = enemy->getScore() / 1000.f;

	// Resources (4)
	inputData[e(I::RES_FOOD)] = res->getValue(ResourceType::FOOD) / 1000.f;
	inputData[e(I::RES_WOOD)] = res->getValue(ResourceType::WOOD) / 1000.f;
	inputData[e(I::RES_STONE)] = res->getValue(ResourceType::STONE) / 1000.f;
	inputData[e(I::RES_GOLD)] = res->getValue(ResourceType::GOLD) / 1000.f;

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	BuildSpatialOutput output;
	for (int i = 0; i < AI_MAP_COUNT; ++i) {
		output.weights[i] = result[i];
	}
	return output;
}
