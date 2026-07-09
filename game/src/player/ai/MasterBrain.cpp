#include "MasterBrain.h"

#include <magic_enum.hpp>
#include "AiHistory.h"
#include "AiUtils.h"
#include "NormScale.h"
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
	: brain(BrainProvider::get(nation->brainPrefix[0] + "master.csv")), nation(nation) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<MasterInputIdx>());
	assert(brain->getOutputSize() == magic_enum::enum_count<MasterOutputIdx>());
}

MasterOutput MasterBrain::decide(Player* player, Player* enemy, float totalLacking, const AiHistory* history) {
	float resSum = sumSpan(player->getResources()->getValues());
	float gatherSum = sumSpan(player->getResources()->getGatherSpeeds());

	float deltaScore = norm(player->getScore() - prevScore, NormScale::SCORE);
	float deltaEnemyScore = norm(enemy->getScore() - prevEnemyScore, NormScale::SCORE);
	float deltaUnits = norm(player->getPossession()->getUnitsNumber() - prevUnits, NormScale::ARMY);
	float deltaRes = norm(resSum - prevResSum, NormScale::RES);
	float deltaGatherSpeed = norm(gatherSum - prevGatherSum, NormScale::GATHER_DELTA);

	// Build input array
	using I = MasterInputIdx;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();
	auto* res = player->getResources();

	// Scores
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), NormScale::SCORE);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), NormScale::SCORE);

	// Counts
	inputData[idx(I::BUILDINGS_COUNT)] = norm(possession->getBuildingsNumber(), NormScale::BUILDINGS);
	inputData[idx(I::WORKERS_COUNT)] = norm(possession->getWorkersNumber(), NormScale::WORKERS);
	inputData[idx(I::ARMY_COUNT)] = norm(possession->getArmyNumber(), NormScale::ARMY);
	inputData[idx(I::ENEMY_ARMY_COUNT)] = norm(enemyPossession->getArmyNumber(), NormScale::ARMY);

	// Resources — stockpiles
	inputData[idx(I::RES_FOOD)] = norm(res->getValue(ResourceType::FOOD), NormScale::RES);
	inputData[idx(I::RES_WOOD)] = norm(res->getValue(ResourceType::WOOD), NormScale::RES);
	inputData[idx(I::RES_STONE)] = norm(res->getValue(ResourceType::STONE), NormScale::RES);
	inputData[idx(I::RES_GOLD)] = norm(res->getValue(ResourceType::GOLD), NormScale::RES);

	// Resources — gather speeds
	inputData[idx(I::GATHER_FOOD)] = norm(res->getGatherSpeed(ResourceType::FOOD), NormScale::GATHER);
	inputData[idx(I::GATHER_WOOD)] = norm(res->getGatherSpeed(ResourceType::WOOD), NormScale::GATHER);
	inputData[idx(I::GATHER_STONE)] = norm(res->getGatherSpeed(ResourceType::STONE), NormScale::GATHER);
	inputData[idx(I::GATHER_GOLD)] = norm(res->getGatherSpeed(ResourceType::GOLD), NormScale::GATHER);

	// Attack/Defence
	inputData[idx(I::ATTACK_SUM)] = norm(possession->getAttackSum(), NormScale::ATTACK);
	inputData[idx(I::DEFENCE_SUM)] = norm(possession->getDefenceAttackSum(), NormScale::DEFENCE);

	// Spatial distances
	inputData[idx(I::DIST_OUR_ARMY_OUR_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, player, 0.f);
	inputData[idx(I::DIST_OUR_ARMY_ENEMY_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, enemy, 1.f);
	inputData[idx(I::DIST_ENEMY_ARMY_OUR_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);
	inputData[idx(I::DIST_ENEMY_ARMY_ENEMY_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, enemy, 0.f);

	// Lacking feedback
	inputData[idx(I::TOTAL_LACKING)] = norm(totalLacking, NormScale::TOTAL_LACKING);

	// Deltas
	inputData[idx(I::DELTA_SCORE)] = deltaScore;
	inputData[idx(I::DELTA_ENEMY_SCORE)] = deltaEnemyScore;
	inputData[idx(I::DELTA_UNITS)] = deltaUnits;
	inputData[idx(I::DELTA_RES)] = deltaRes;
	inputData[idx(I::DELTA_GATHER_SPEED)] = deltaGatherSpeed;

	inputData[idx(I::GAME_TIME)] = norm(Game::getFrameInfo()->getSeconds(), NormScale::GAME_TIME);
	float killed = possession->getValueDestroyed();
	float lost = enemyPossession->getValueDestroyed();
	inputData[idx(I::KD_RATIO)] = norm(killed, std::max(killed + lost, 1.f));
	inputData[idx(I::IN_COMBAT_RATIO)] = possession->getInCombatRatio();
	// Overall tech progress: pooled average upgrade level across all units and buildings ([0,1]).
	inputData[idx(I::TECH_LEVEL)] = avgTechLevel(nation->units, nation->buildings, player);

	// History inputs (lookback ~30 seconds at 30 ticks/s = 900 ticks)
	constexpr unsigned int LOOKBACK = 900;
	inputData[idx(I::RECENT_BUILD_FAILURES)] = norm(history->buildingFailureScore(LOOKBACK), NormScale::BUILD_FAILURE);
	inputData[idx(I::RECENT_UNIT_FAILURES)] = norm(history->unitFailureScore(LOOKBACK), NormScale::FAILURE);
	inputData[idx(I::RECENT_ATTACK_ACTIVITY)] = norm(history->attackActivityScore(LOOKBACK), NormScale::ACTIVITY);
	inputData[idx(I::RECENT_DEFEND_ACTIVITY)] = norm(history->defendActivityScore(LOOKBACK), NormScale::ACTIVITY);

	updateHistory(player, enemy);

	auto result = brain->decide(inputData);

	using O = MasterOutputIdx;
	return MasterOutput{
			.workerUrgency = result[idx(O::WORKER_URGENCY)],
			.economyUrgency = result[idx(O::ECONOMY_URGENCY)],
			.buildingUrgency = result[idx(O::BUILDING_URGENCY)],
			.unitUrgency = result[idx(O::UNIT_URGENCY)],
			.militaryUrgency = result[idx(O::MILITARY_URGENCY)],
			.expandUrgency = result[idx(O::EXPAND_URGENCY)],
			.techUrgency = result[idx(O::TECH_URGENCY)],
			.defenceBuildingUrgency = result[idx(O::DEFENCE_BUILDING_URGENCY)],
			.attackUrgency = result[idx(O::ATTACK_URGENCY)]
	};
}

void MasterBrain::updateHistory(Player* player, Player* enemy) {
	prevScore = player->getScore();
	prevEnemyScore = enemy->getScore();
	prevUnits = player->getPossession()->getUnitsNumber();
	prevResSum = sumSpan(player->getResources()->getValues());
	prevGatherSum = sumSpan(player->getResources()->getGatherSpeeds());
}
