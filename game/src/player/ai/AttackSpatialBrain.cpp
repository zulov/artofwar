#include "AttackSpatialBrain.h"

#include <algorithm>
#include "AiUtils.h"
#include "NormScale.h"
#include <magic_enum.hpp>
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"

AttackSpatialBrain::AttackSpatialBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->orderPrefix[0] + "attack_spatial.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<AttackSpatialInputIdx>());
	assert(brain->getOutputSize() == magic_enum::enum_count<AttackSpatialOutputIdx>());
}

AttackSpatialOutput AttackSpatialBrain::decide(Player* player, Player* enemy,
                                                float militaryUrgency, float attackUrgency) {
	using I = AttackSpatialInputIdx;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();

	// Army counts (3)
	inputData[idx(I::ARMY_COUNT)] = norm(possession->getArmyNumber(), NormScale::ARMY);
	inputData[idx(I::FREE_ARMY_COUNT)] = norm(possession->getFreeArmyNumber(), NormScale::ARMY);
	inputData[idx(I::ENEMY_ARMY_COUNT)] = norm(enemyPossession->getArmyNumber(), NormScale::ARMY);

	// Scores (2)
	inputData[idx(I::PLAYER_SCORE)] = norm(player->getScore(), NormScale::SCORE);
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), NormScale::SCORE);

	// Military strength (2)
	inputData[idx(I::ATTACK_SUM)] = norm(possession->getAttackSum(), NormScale::ATTACK);
	inputData[idx(I::DEFENCE_SUM)] = norm(possession->getDefenceAttackSum(), NormScale::DEFENCE);

	// Combat state (1)
	inputData[idx(I::IN_COMBAT_RATIO)] = possession->getInCombatRatio();

	// Urgencies from MasterBrain (2)
	inputData[idx(I::MILITARY_URGENCY)] = militaryUrgency;
	inputData[idx(I::ATTACK_URGENCY)] = attackUrgency;

	auto result = brain->decide(inputData);

	AttackSpatialOutput output;
	std::copy_n(result.begin(), AI_ARMY_MAP_COUNT, output.weights.begin());
	return output;
}
