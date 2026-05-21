#include "UnitBrain.h"

#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"

#include <algorithm>
#include <cmath>

UnitBrain::UnitBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->actionPrefix[3] + "unit.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getOutputSize() == static_cast<int>(UnitOutputIdx::COUNT));
}

UnitOutput UnitBrain::decide(Player* player, Player* enemy,
                              float unitUrgency, float attackUrgency) {
	using I = UnitInputIdx;
	constexpr auto e = [](I v) { return static_cast<int>(v); };

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();

	// Our army composition — for counter-picking (3)
	float armyCount = static_cast<float>(possession->getArmyNumber());
	float safeDiv = std::max(armyCount, 1.f);
	inputData[e(I::OUR_INFANTRY_RATIO)] = static_cast<float>(possession->getInfantryNumber()) / safeDiv;
	inputData[e(I::OUR_CAVALRY_RATIO)] = static_cast<float>(possession->getCavalryNumber()) / safeDiv;
	inputData[e(I::OUR_RANGE_RATIO)] = static_cast<float>(possession->getRangeNumber()) / safeDiv;

	// Enemy army composition — for counter-picking (3)
	float enemyArmyCount = static_cast<float>(enemyPossession->getArmyNumber());
	float enemySafeDiv = std::max(enemyArmyCount, 1.f);
	inputData[e(I::ENEMY_INFANTRY_RATIO)] = static_cast<float>(enemyPossession->getInfantryNumber()) / enemySafeDiv;
	inputData[e(I::ENEMY_CAVALRY_RATIO)] = static_cast<float>(enemyPossession->getCavalryNumber()) / enemySafeDiv;
	inputData[e(I::ENEMY_RANGE_RATIO)] = static_cast<float>(enemyPossession->getRangeNumber()) / enemySafeDiv;

	// Army counts (2)
	inputData[e(I::ARMY_COUNT)] = armyCount / 200.f;
	inputData[e(I::ENEMY_ARMY_COUNT)] = enemyArmyCount / 200.f;

	// Military strength (2)
	inputData[e(I::ATTACK_SUM)] = possession->getAttackSum() / 1000.f;
	inputData[e(I::DEFENCE_ATTACK_SUM)] = possession->getDefenceAttackSum() / 100.f;

	// Enemy score (1)
	inputData[e(I::ENEMY_SCORE)] = static_cast<float>(enemy->getScore()) / 1000.f;

	// Worker count — army vs worker balance (1)
	inputData[e(I::WORKERS_COUNT)] = static_cast<float>(possession->getWorkersNumber()) / 100.f;

	// Urgencies from Master (2)
	inputData[e(I::UNIT_URGENCY)] = unitUrgency;
	inputData[e(I::ATTACK_URGENCY)] = attackUrgency;

	auto result = brain->decide(std::span<const float>(inputData.data(), inputData.size()));

	UnitOutput output{};
	for (int i = 0; i < 24; ++i) {
		output.unitProfile[i] = result[i];
	}

	// Count scaling
	if (unitUrgency > 0.f) {
		int raw = static_cast<int>(std::round(unitUrgency * MAX_UNITS_PER_TICK));
		output.count = static_cast<unsigned char>(std::max(1, raw));
	} else {
		output.count = 0;
	}

	return output;
}
