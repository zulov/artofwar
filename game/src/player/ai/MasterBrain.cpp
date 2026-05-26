#include "MasterBrain.h"

#include <magic_enum.hpp>
#include "AiHistory.h"
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

MasterOutput MasterBrain::decide(Player* player, Player* enemy, float totalLacking, const AiHistory* history) {
	float resSum = sumSpan(player->getResources()->getValues());
	float gatherSum = sumSpan(player->getResources()->getGatherSpeeds());

	float deltaScore = norm(player->getScore() - prevScore, 1000.f);
	float deltaEnemyScore = norm(enemy->getScore() - prevEnemyScore, 1000.f);
	float deltaUnits = norm(player->getPossession()->getUnitsNumber() - prevUnits, 200.f);
	float deltaRes = norm(resSum - prevResSum, 1000.f);
	float deltaGatherSpeed = norm(gatherSum - prevGatherSum, 20.f);

	// Build input array
	using I = MasterInputIdx;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();
	auto* res = player->getResources();

	// Scores
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), 1000.f);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), 1000.f);

	// Counts
	inputData[idx(I::BUILDINGS_COUNT)] = norm(possession->getBuildingsNumber(), 50.f);
	inputData[idx(I::WORKERS_COUNT)] = norm(possession->getWorkersNumber(), 100.f);
	inputData[idx(I::ARMY_COUNT)] = norm(possession->getArmyNumber(), 200.f);
	inputData[idx(I::ENEMY_ARMY_COUNT)] = norm(enemyPossession->getArmyNumber(), 200.f);

	// Resources — stockpiles
	inputData[idx(I::RES_FOOD)] = norm(res->getValue(ResourceType::FOOD), 1000.f);
	inputData[idx(I::RES_WOOD)] = norm(res->getValue(ResourceType::WOOD), 1000.f);
	inputData[idx(I::RES_STONE)] = norm(res->getValue(ResourceType::STONE), 1000.f);
	inputData[idx(I::RES_GOLD)] = norm(res->getValue(ResourceType::GOLD), 1000.f);

	// Resources — gather speeds
	inputData[idx(I::GATHER_FOOD)] = norm(res->getGatherSpeed(ResourceType::FOOD), 10.f);
	inputData[idx(I::GATHER_WOOD)] = norm(res->getGatherSpeed(ResourceType::WOOD), 10.f);
	inputData[idx(I::GATHER_STONE)] = norm(res->getGatherSpeed(ResourceType::STONE), 10.f);
	inputData[idx(I::GATHER_GOLD)] = norm(res->getGatherSpeed(ResourceType::GOLD), 10.f);

	// Attack/Defence
	inputData[idx(I::ATTACK_SUM)] = norm(possession->getAttackSum(), 1000.f);
	inputData[idx(I::DEFENCE_SUM)] = norm(possession->getDefenceAttackSum(), 100.f);

	// Spatial distances
	inputData[idx(I::DIST_OUR_ARMY_OUR_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, player, 0.f);
	inputData[idx(I::DIST_OUR_ARMY_ENEMY_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, enemy, 1.f);
	inputData[idx(I::DIST_ENEMY_ARMY_OUR_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);
	inputData[idx(I::DIST_ENEMY_ARMY_ENEMY_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, enemy, 0.f);

	// Lacking feedback
	inputData[idx(I::TOTAL_LACKING)] = norm(totalLacking, 2000.f);

	// Deltas
	inputData[idx(I::DELTA_SCORE)] = deltaScore;
	inputData[idx(I::DELTA_ENEMY_SCORE)] = deltaEnemyScore;
	inputData[idx(I::DELTA_UNITS)] = deltaUnits;
	inputData[idx(I::DELTA_RES)] = deltaRes;
	inputData[idx(I::DELTA_GATHER_SPEED)] = deltaGatherSpeed;

	inputData[idx(I::GAME_TIME)] = norm(Game::getFrameInfo()->getSeconds(), 1800.f);
	float killed = possession->getValueDestroyed();
	float lost = enemyPossession->getValueDestroyed();
	inputData[idx(I::KD_RATIO)] = norm(killed, std::max(killed + lost, 1.f));
	inputData[idx(I::IN_COMBAT_RATIO)] = possession->getInCombatRatio();
	inputData[idx(I::TECH_LEVEL)] = 0.f; //TODO implement — no tech API exists yet

	// History inputs (lookback ~30 seconds at 30 ticks/s = 900 ticks)
	constexpr unsigned int LOOKBACK = 900;
	float buildFailures = history->failureScore(AiActionType::CREATE_BUILDING_OTHER, LOOKBACK)
		+ history->failureScore(AiActionType::CREATE_BUILDING_DEFENCE, LOOKBACK)
		+ history->failureScore(AiActionType::CREATE_BUILDING_RESOURCE, LOOKBACK)
		+ history->failureScore(AiActionType::CREATE_BUILDING_TECH, LOOKBACK)
		+ history->failureScore(AiActionType::CREATE_BUILDING_UNITS, LOOKBACK);
	inputData[idx(I::RECENT_BUILD_FAILURES)] = norm(buildFailures, 10.f);
	inputData[idx(I::RECENT_UNIT_FAILURES)] = norm(history->failureScore(AiActionType::CREATE_UNIT, LOOKBACK), 5.f);
	float attackActivity = history->recencyScore(AiOrderType::ATTACK_ECON, LOOKBACK)
		+ history->recencyScore(AiOrderType::ATTACK_BUILDING, LOOKBACK)
		+ history->recencyScore(AiOrderType::ATTACK_ARMY, LOOKBACK);
	inputData[idx(I::RECENT_ATTACK_ACTIVITY)] = norm(attackActivity, 10.f);
	float defendActivity = history->recencyScore(AiOrderType::DEFEND_ECON, LOOKBACK)
		+ history->recencyScore(AiOrderType::DEFEND_BUILDING, LOOKBACK)
		+ history->recencyScore(AiOrderType::DEFEND_ARMY, LOOKBACK);
	inputData[idx(I::RECENT_DEFEND_ACTIVITY)] = norm(defendActivity, 10.f);

	updateHistory(player, enemy);

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	return MasterOutput{
		result[0], result[1], result[2], result[3],
		result[4], result[5], result[6], result[7], result[8]
	};
}

void MasterBrain::updateHistory(Player* player, Player* enemy) {
	prevScore = player->getScore();
	prevEnemyScore = enemy->getScore();
	prevUnits = player->getPossession()->getUnitsNumber();
	prevResSum = sumSpan(player->getResources()->getValues());
	prevGatherSum = sumSpan(player->getResources()->getGatherSpeeds());
}
