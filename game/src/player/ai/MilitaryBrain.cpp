#include "MilitaryBrain.h"

#include <algorithm>
#include <magic_enum.hpp>
#include "AiHistory.h"
#include "AiUtils.h"
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
	assert(brain->getOutputSize() == magic_enum::enum_count<MilitaryOutputIdx>());
}

MilitaryOutput MilitaryBrain::decide(Player* player, Player* enemy,
                                      float militaryUrgency, float attackUrgency,
                                      const AiHistory* history) {
	using I = MilitaryInputIdx;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();

	// Scores (2)
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), 1000.f);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), 1000.f);

	// Military strength (2)
	inputData[idx(I::ATTACK_SUM)] = norm(possession->getAttackSum(), 1000.f);
	inputData[idx(I::DEFENCE_SUM)] = norm(possession->getDefenceAttackSum(), 100.f);

	// Spatial — army/building distances (4)
	inputData[idx(I::DIST_OUR_ARMY_OUR_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, player, 0.f);
	inputData[idx(I::DIST_OUR_ARMY_ENEMY_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, enemy, 1.f);
	inputData[idx(I::DIST_ENEMY_ARMY_OUR_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);
	inputData[idx(I::DIST_ENEMY_ARMY_ENEMY_BUILDING)] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, enemy, 0.f);

	// Army counts (3)
	inputData[idx(I::ARMY_COUNT)] = norm(possession->getArmyNumber(), 200.f);
	inputData[idx(I::ENEMY_ARMY_COUNT)] = norm(enemyPossession->getArmyNumber(), 200.f);
	inputData[idx(I::FREE_ARMY_COUNT)] = norm(possession->getFreeArmyNumber(), 200.f);

	// Urgencies from Master (2)
	inputData[idx(I::MILITARY_URGENCY)] = militaryUrgency;
	inputData[idx(I::ATTACK_URGENCY)] = attackUrgency;

	// Army composition ratios — TODO implement
	float armyCount = static_cast<float>(possession->getArmyNumber());
	float safeDiv = std::max(armyCount, 1.f);
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
	float attackFailures = history->failureScore(AiOrderType::ATTACK_ECON, LOOKBACK)
		+ history->failureScore(AiOrderType::ATTACK_BUILDING, LOOKBACK)
		+ history->failureScore(AiOrderType::ATTACK_ARMY, LOOKBACK);
	inputData[idx(I::RECENT_ATTACK_FAILURES)] = norm(attackFailures, 5.f);
	float defendFailures = history->failureScore(AiOrderType::DEFEND_ECON, LOOKBACK)
		+ history->failureScore(AiOrderType::DEFEND_BUILDING, LOOKBACK)
		+ history->failureScore(AiOrderType::DEFEND_ARMY, LOOKBACK);
	inputData[idx(I::RECENT_DEFEND_FAILURES)] = norm(defendFailures, 5.f);

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	return MilitaryOutput{
		result[0], result[1], result[2], result[3]
	};
}
