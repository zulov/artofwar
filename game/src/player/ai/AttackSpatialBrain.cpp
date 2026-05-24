#include "AttackSpatialBrain.h"

#include "AiUtils.h"
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
                                                const MilitaryOutput& milOut,
                                                float militaryUrgency, float attackUrgency) {
	using I = AttackSpatialInputIdx;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();

	// MilitaryBrain outputs (4)
	inputData[idx(I::ATTACK_WEIGHT)] = milOut.attackWeight;
	inputData[idx(I::DEFEND_WEIGHT)] = milOut.defendWeight;
	inputData[idx(I::ATTACK_TARGET)] = milOut.attackTarget;
	inputData[idx(I::DEFEND_TARGET)] = milOut.defendTarget;

	// Army counts (3)
	inputData[idx(I::ARMY_COUNT)] = static_cast<float>(possession->getArmyNumber()) / 200.f;
	inputData[idx(I::FREE_ARMY_COUNT)] = static_cast<float>(possession->getFreeArmyNumber()) / 200.f;
	inputData[idx(I::ENEMY_ARMY_COUNT)] = static_cast<float>(enemyPossession->getArmyNumber()) / 200.f;

	// Scores (2)
	inputData[idx(I::PLAYER_SCORE)] = player->getScore() / 1000.f;
	inputData[idx(I::ENEMY_SCORE)] = enemy->getScore() / 1000.f;

	// Military strength (2)
	inputData[idx(I::ATTACK_SUM)] = possession->getAttackSum() / 1000.f;
	inputData[idx(I::DEFENCE_SUM)] = possession->getDefenceAttackSum() / 100.f;

	// Combat state (1)
	inputData[idx(I::IN_COMBAT_RATIO)] = 0.f; //TODO implement

	// Urgencies from MasterBrain (2)
	inputData[idx(I::MILITARY_URGENCY)] = militaryUrgency;
	inputData[idx(I::ATTACK_URGENCY)] = attackUrgency;

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	AttackSpatialOutput output;
	for (int i = 0; i < AI_ARMY_MAP_COUNT; ++i) {
		output.weights[i] = result[i];
	}
	return output;
}
