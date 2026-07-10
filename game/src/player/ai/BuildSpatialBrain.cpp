#include "BuildSpatialBrain.h"

#include <algorithm>
#include "AiUtils.h"
#include "NormScale.h"
#include <magic_enum.hpp>
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "database/db_struct.h"
#include "objects/building/BuildPlacementClass.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "Game.h"

BuildSpatialBrain::BuildSpatialBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->brainPrefix[2] + "build_spatial.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<BuildSpatialInputIdx>());
	assert(brain->getOutputSize() == magic_enum::enum_count<BuildSpatialOutputIdx>() ||
	       brain->getOutputSize() == 15);
}

BuildSpatialOutput BuildSpatialBrain::decide(Player* player, Player* enemy,
                                              float expandUrgency,
                                              const db_building* building, const db_building_level* level) {
	using I = BuildSpatialInputIdx;

	auto* possession = player->getPossession();

	// Urgencies from MasterBrain (1)
	inputData[idx(I::EXPAND_URGENCY)] = expandUrgency;

	// Counts (3)
	inputData[idx(I::BUILDINGS_COUNT)] = norm(possession->getBuildingsNumber(), NormScale::BUILDINGS);
	inputData[idx(I::WORKERS_COUNT)] = norm(possession->getWorkersNumber(), NormScale::WORKERS);
	inputData[idx(I::ARMY_COUNT)] = norm(possession->getArmyNumber(), NormScale::ARMY);

	// Game state (3)
	inputData[idx(I::GAME_TIME)] = norm(Game::getFrameInfo()->getSeconds(), NormScale::GAME_TIME);
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), NormScale::SCORE);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), NormScale::SCORE);

	// Placement class one-hot derived from the chosen building's DB data.
	const BuildPlacementClass placementClass = classifyPlacement(building, level);
	const auto placementIndex = static_cast<unsigned char>(placementClass);
	for (unsigned char c = 0; c < BUILD_PLACEMENT_CLASS_COUNT; ++c) {
		inputData[idx(I::CLASS_OTHER) + c] = c == placementIndex ? 1.f : 0.f;
	}

	inputData[idx(I::RES_TYPE_FOOD)] = building->typeResourceFood ? 1.f : 0.f;
	inputData[idx(I::RES_TYPE_WOOD)] = building->typeResourceWood ? 1.f : 0.f;
	inputData[idx(I::RES_TYPE_STONE)] = building->typeResourceStone ? 1.f : 0.f;
	inputData[idx(I::RES_TYPE_GOLD)] = building->typeResourceGold ? 1.f : 0.f;

	auto result = brain->decide(inputData);

	BuildSpatialOutput output;
	if (result.size() == magic_enum::enum_count<BuildSpatialOutputIdx>()) {
		std::copy_n(result.begin(), AI_MAP_COUNT, output.weights.begin());
	} else {
		std::copy_n(result.begin(), 10, output.weights.begin());
		output.weights[idx(BuildSpatialOutputIdx::W_RES_NOT_BONUS)] =
				(result[10] + result[11] + result[12] + result[13]) * 0.25f;
		output.weights[idx(BuildSpatialOutputIdx::W_ECONOMY)] = result[14];
	}
	return output;
}
