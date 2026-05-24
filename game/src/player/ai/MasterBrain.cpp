#include "MasterBrain.h"

#include <magic_enum.hpp>
#include "AiUtils.h"
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "MetricDefinitions.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"
#include "player/PlayersManager.h"
#include "Game.h"
#include "env/influence/CenterType.h"

MasterBrain::MasterBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->actionPrefix[0] + "master.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<MasterInputIdx>());
	assert(brain->getOutputSize() == magic_enum::enum_count<MasterOutputIdx>());
}

MasterOutput MasterBrain::decide(Player* player, Player* enemy, float totalLacking) {
	float deltaScore = norm(player->getScore() - prevScore, 1000.f);
	float deltaUnits = norm(static_cast<float>(player->getPossession()->getUnitsNumber()) - prevUnits, 200.f);
	float resSum = 0.f;
	for (int i = 0; i < 4; ++i) {
		resSum += player->getResources()->getValues()[i];
	}
	float deltaRes = norm(resSum - prevResSum, 1000.f);
	float normTotalLacking = norm(totalLacking, 2000.f);

	// Build input array
	using I = MasterInputIdx;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();
	auto* res = player->getResources();

	// Scores (2)
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), 1000.f);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), 1000.f);

	// Counts (4)
	inputData[idx(I::BUILDINGS_COUNT)] = norm(possession->getBuildingsNumber(), 50.f);
	inputData[idx(I::WORKERS_COUNT)] = norm(possession->getWorkersNumber(), 100.f);
	inputData[idx(I::ARMY_COUNT)] = norm(possession->getArmyNumber(), 200.f);
	inputData[idx(I::ENEMY_ARMY_COUNT)] = norm(enemyPossession->getArmyNumber(), 200.f);

	// Resources — stockpiles (4)
	inputData[idx(I::RES_FOOD)] = norm(res->getValue(ResourceType::FOOD), 1000.f);
	inputData[idx(I::RES_WOOD)] = norm(res->getValue(ResourceType::WOOD), 1000.f);
	inputData[idx(I::RES_STONE)] = norm(res->getValue(ResourceType::STONE), 1000.f);
	inputData[idx(I::RES_GOLD)] = norm(res->getValue(ResourceType::GOLD), 1000.f);

	// Resources — gather speeds (4)
	inputData[idx(I::GATHER_FOOD)] = norm(res->getGatherSpeed(ResourceType::FOOD), 10.f);
	inputData[idx(I::GATHER_WOOD)] = norm(res->getGatherSpeed(ResourceType::WOOD), 10.f);
	inputData[idx(I::GATHER_STONE)] = norm(res->getGatherSpeed(ResourceType::STONE), 10.f);
	inputData[idx(I::GATHER_GOLD)] = norm(res->getGatherSpeed(ResourceType::GOLD), 10.f);

	// Attack/Defence (2)
	inputData[idx(I::ATTACK_SUM)] = norm(possession->getAttackSum(), 1000.f);
	inputData[idx(I::DEFENCE_SUM)] = norm(possession->getDefenceAttackSum(), 100.f);

	// Spatial distances (4)
	inputData[idx(I::DIST_OUR_ARMY_OUR_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, player, 0.f);
	inputData[idx(I::DIST_OUR_ARMY_ENEMY_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, enemy, 1.f);
	inputData[idx(I::DIST_ENEMY_ARMY_OUR_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);
	inputData[idx(I::DIST_ENEMY_ARMY_ENEMY_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, enemy, 0.f);

	// Lacking feedback (1)
	inputData[idx(I::TOTAL_LACKING)] = normTotalLacking;

	// Deltas (3)
	inputData[idx(I::DELTA_SCORE)] = deltaScore;
	inputData[idx(I::DELTA_ENEMY_SCORE)] = 0.f; //TODO implement
	inputData[idx(I::DELTA_UNITS)] = deltaUnits;
	inputData[idx(I::DELTA_RES)] = deltaRes;

	// New inputs — TODO implement
	inputData[idx(I::GAME_TIME)] = 0.f; //TODO implement
	inputData[idx(I::KD_RATIO)] = 0.f; //TODO implement
	inputData[idx(I::IN_COMBAT_RATIO)] = 0.f; //TODO implement
	inputData[idx(I::TECH_LEVEL)] = 0.f; //TODO implement
	inputData[idx(I::DELTA_GATHER_SPEED)] = 0.f; //TODO implement

	updateHistory(player);

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	return MasterOutput{
		result[0], result[1], result[2], result[3],
		result[4], result[5], result[6], result[7], result[8]
	};
}

void MasterBrain::updateHistory(Player* player) {
	prevScore = player->getScore();
	prevUnits = static_cast<float>(player->getPossession()->getUnitsNumber());
	prevResSum = 0.f;
	for (int i = 0; i < 4; ++i) {
		prevResSum += player->getResources()->getValues()[i];
	}
}
