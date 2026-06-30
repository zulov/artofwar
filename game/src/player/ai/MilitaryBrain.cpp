#include "MilitaryBrain.h"

#include <algorithm>
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
#include "env/influence/CenterType.h"

MilitaryBrain::MilitaryBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->orderPrefix[0] + "military.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<MilitaryInputIdx>());
	assert(brain->getOutputSize() == MILITARY_OUTPUT_COUNT);
}

MilitaryOutput MilitaryBrain::decide(Player* player, Player* enemy,
                                      float militaryUrgency, float attackUrgency,
                                      float techLevel,
                                      const AiHistory* history) {
	using I = MilitaryInputIdx;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();

	// Scores (2)
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), NormScale::SCORE);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), NormScale::SCORE);

	// Military strength (2)
	inputData[idx(I::ATTACK_SUM)] = norm(possession->getAttackSum(), NormScale::ATTACK);
	inputData[idx(I::DEFENCE_SUM)] = norm(possession->getDefenceAttackSum(), NormScale::DEFENCE);

	// Spatial — army/building distances (4)
	inputData[idx(I::DIST_OUR_ARMY_OUR_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, player, 0.f);
	inputData[idx(I::DIST_OUR_ARMY_ENEMY_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, enemy, 1.f);
	inputData[idx(I::DIST_ENEMY_ARMY_OUR_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);
	inputData[idx(I::DIST_ENEMY_ARMY_ENEMY_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, enemy, 0.f);

	// Army counts (4)
	inputData[idx(I::ARMY_COUNT)] = norm(possession->getArmyNumber(), NormScale::ARMY);
	inputData[idx(I::ENEMY_ARMY_COUNT)] = norm(enemyPossession->getArmyNumber(), NormScale::ARMY);
	const auto& armyByActivity = possession->getArmyByActivity();
	inputData[idx(I::FREE_ARMY_COUNT)] = norm(static_cast<unsigned>(armyByActivity.idle.size()), NormScale::ARMY);
	inputData[idx(I::FREE_ARMY_ATTACK_SUM)] = norm(possession->getFreeArmyAttackSum(), NormScale::ATTACK);

	// Army activity ratios (fraction of our army currently attacking / defending)
	float armyCount = static_cast<float>(possession->getArmyNumber());
	float safeDiv = std::max(armyCount, 1.f);
	inputData[idx(I::ARMY_ATTACKING_RATIO)] = norm(static_cast<unsigned>(armyByActivity.attacking.size()), safeDiv);
	inputData[idx(I::ARMY_DEFENDING_RATIO)] = norm(static_cast<unsigned>(armyByActivity.defending.size()), safeDiv);

	// Urgencies from Master (2)
	inputData[idx(I::MILITARY_URGENCY)] = militaryUrgency;
	inputData[idx(I::ATTACK_URGENCY)] = attackUrgency;

	// Army composition ratios
	inputData[idx(I::OWN_INFANTRY_RATIO)] = norm(possession->getInfantryNumber(), safeDiv);
	inputData[idx(I::OWN_RANGE_RATIO)] = norm(possession->getRangeNumber(), safeDiv);
	inputData[idx(I::OWN_CAVALRY_RATIO)] = norm(possession->getCavalryNumber(), safeDiv);

	float enemyArmyCount = static_cast<float>(enemyPossession->getArmyNumber());
	float enemySafeDiv = std::max(enemyArmyCount, 1.f);
	inputData[idx(I::ENEMY_INFANTRY_RATIO)] = norm(enemyPossession->getInfantryNumber(), enemySafeDiv);
	inputData[idx(I::ENEMY_RANGE_RATIO)] = norm(enemyPossession->getRangeNumber(), enemySafeDiv);
	inputData[idx(I::ENEMY_CAVALRY_RATIO)] = norm(enemyPossession->getCavalryNumber(), enemySafeDiv);

	// History — order failures signal unreachable targets
	constexpr unsigned int LOOKBACK = 1800;
	inputData[idx(I::RECENT_ATTACK_FAILURES)] = norm(history->attackFailureScore(LOOKBACK), NormScale::FAILURE);
	inputData[idx(I::RECENT_DEFEND_FAILURES)] = norm(history->defendFailureScore(LOOKBACK), NormScale::FAILURE);
	inputData[idx(I::RECENT_ATTACK_ACTIVITY)] = norm(history->attackActivityScore(LOOKBACK), NormScale::ACTIVITY);
	inputData[idx(I::RECENT_DEFEND_ACTIVITY)] = norm(history->defendActivityScore(LOOKBACK), NormScale::ACTIVITY);

	// Overall tech progress ([0,1]) — lets the brain weight aggression by tech lead/deficit
	inputData[idx(I::TECH_LEVEL)] = techLevel;

	auto result = brain->decide(inputData);

	MilitaryOutput output;
	std::copy_n(result.begin(), output.centerPairPressure.size(), output.centerPairPressure.begin());
	output.preferInfantry = result[idx(MilitaryPreferIdx::INFANTRY)];
	output.preferRange = result[idx(MilitaryPreferIdx::RANGE)];
	output.preferCavalry = result[idx(MilitaryPreferIdx::CAVALRY)];
	return output;
}
