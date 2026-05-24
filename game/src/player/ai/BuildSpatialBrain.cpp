#include "BuildSpatialBrain.h"

#include "AiUtils.h"
#include <magic_enum.hpp>
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"
#include "Game.h"

BuildSpatialBrain::BuildSpatialBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->actionPrefix[0] + "build_spatial.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<BuildSpatialInputIdx>());
	assert(brain->getOutputSize() == magic_enum::enum_count<BuildSpatialOutputIdx>());
}

BuildSpatialOutput BuildSpatialBrain::decide(Player* player, Player* enemy,
                                              float buildingUrgency, float expandUrgency, float defenceBuildingUrgency) {
	using I = BuildSpatialInputIdx;

	auto* possession = player->getPossession();
	auto* res = player->getResources();

	// Urgencies from MasterBrain (3)
	inputData[idx(I::BUILDING_URGENCY)] = buildingUrgency;
	inputData[idx(I::EXPAND_URGENCY)] = expandUrgency;
	inputData[idx(I::DEFENCE_BUILDING_URGENCY)] = defenceBuildingUrgency;

	// Counts (3)
	inputData[idx(I::BUILDINGS_COUNT)] = norm(possession->getBuildingsNumber(), 50.f);
	inputData[idx(I::WORKERS_COUNT)] = norm(possession->getWorkersNumber(), 100.f);
	inputData[idx(I::ARMY_COUNT)] = norm(possession->getArmyNumber(), 200.f);

	// Game state (3)
	inputData[idx(I::GAME_TIME)] = norm(Game::getFrameInfo()->getSeconds(), 1800.f);
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), 1000.f);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), 1000.f);

	// Resources (4)
	inputData[idx(I::RES_FOOD)] = norm(res->getValue(ResourceType::FOOD), 1000.f);
	inputData[idx(I::RES_WOOD)] = norm(res->getValue(ResourceType::WOOD), 1000.f);
	inputData[idx(I::RES_STONE)] = norm(res->getValue(ResourceType::STONE), 1000.f);
	inputData[idx(I::RES_GOLD)] = norm(res->getValue(ResourceType::GOLD), 1000.f);

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	BuildSpatialOutput output;
	for (int i = 0; i < AI_MAP_COUNT; ++i) {
		output.weights[i] = result[i];
	}
	return output;
}
