#include "AttackSpatialBrain.h"

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
	constexpr auto e = [](I v) { return static_cast<int>(v); };

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();

	// MilitaryBrain outputs (4)
	inputData[e(I::ATTACK_WEIGHT)] = milOut.attackWeight;
	inputData[e(I::DEFEND_WEIGHT)] = milOut.defendWeight;
	inputData[e(I::ATTACK_TARGET)] = milOut.attackTarget;
	inputData[e(I::DEFEND_TARGET)] = milOut.defendTarget;

	// Army counts (3)
	inputData[e(I::ARMY_COUNT)] = static_cast<float>(possession->getArmyNumber()) / 200.f;
	inputData[e(I::FREE_ARMY_COUNT)] = static_cast<float>(possession->getFreeArmyNumber()) / 200.f;
	inputData[e(I::ENEMY_ARMY_COUNT)] = static_cast<float>(enemyPossession->getArmyNumber()) / 200.f;

	// Scores (2)
	inputData[e(I::PLAYER_SCORE)] = player->getScore() / 1000.f;
	inputData[e(I::ENEMY_SCORE)] = enemy->getScore() / 1000.f;

	// Military strength (2)
	inputData[e(I::ATTACK_SUM)] = possession->getAttackSum() / 1000.f;
	inputData[e(I::DEFENCE_SUM)] = possession->getDefenceAttackSum() / 100.f;

	// Combat state (1)
	inputData[e(I::IN_COMBAT_RATIO)] = 0.f; //TODO implement

	// Urgencies from MasterBrain (2)
	inputData[e(I::MILITARY_URGENCY)] = militaryUrgency;
	inputData[e(I::ATTACK_URGENCY)] = attackUrgency;

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	AttackSpatialOutput output;
	for (int i = 0; i < AI_ARMY_MAP_COUNT; ++i) {
		output.weights[i] = result[i];
	}
	return output;
}
