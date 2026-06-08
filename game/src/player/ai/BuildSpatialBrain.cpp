#include "BuildSpatialBrain.h"

#include <algorithm>
#include "AiUtils.h"
#include "NormScale.h"
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
	inputData[idx(I::BUILDINGS_COUNT)] = norm(possession->getBuildingsNumber(), NormScale::BUILDINGS);
	inputData[idx(I::WORKERS_COUNT)] = norm(possession->getWorkersNumber(), NormScale::WORKERS);
	inputData[idx(I::ARMY_COUNT)] = norm(possession->getArmyNumber(), NormScale::ARMY);

	// Game state (3)
	inputData[idx(I::GAME_TIME)] = norm(Game::getFrameInfo()->getSeconds(), NormScale::GAME_TIME);
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), NormScale::SCORE);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), NormScale::SCORE);

	// Resources (4)
	inputData[idx(I::RES_FOOD)] = norm(res->getValue(ResourceType::FOOD), NormScale::RES);
	inputData[idx(I::RES_WOOD)] = norm(res->getValue(ResourceType::WOOD), NormScale::RES);
	inputData[idx(I::RES_STONE)] = norm(res->getValue(ResourceType::STONE), NormScale::RES);
	inputData[idx(I::RES_GOLD)] = norm(res->getValue(ResourceType::GOLD), NormScale::RES);

	auto result = brain->decide(inputData);

	BuildSpatialOutput output;
	std::copy_n(result.begin(), AI_MAP_COUNT, output.weights.begin());
	return output;
}
