#include "UnitBrain.h"

#include "AiUtils.h"
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
	assert(brain->getInputSize() == magic_enum::enum_count<UnitInputIdx>());
	assert(brain->getOutputSize() == static_cast<int>(UnitOutputIdx::COUNT));
}

UnitOutput UnitBrain::decide(Player* player, Player* enemy,
                              float unitUrgency, float attackUrgency) {
	using I = UnitInputIdx;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();

	// Our army composition — for counter-picking (3)
	float armyCount = static_cast<float>(possession->getArmyNumber());
	float safeDiv = std::max(armyCount, 1.f);
	inputData[idx(I::OUR_INFANTRY_RATIO)] = norm(possession->getInfantryNumber(), safeDiv);
	inputData[idx(I::OUR_CAVALRY_RATIO)] = norm(possession->getCavalryNumber(), safeDiv);
	inputData[idx(I::OUR_RANGE_RATIO)] = norm(possession->getRangeNumber(), safeDiv);

	// Enemy army composition — for counter-picking (3)
	float enemyArmyCount = static_cast<float>(enemyPossession->getArmyNumber());
	float enemySafeDiv = std::max(enemyArmyCount, 1.f);
	inputData[idx(I::ENEMY_INFANTRY_RATIO)] = norm(enemyPossession->getInfantryNumber(), enemySafeDiv);
	inputData[idx(I::ENEMY_CAVALRY_RATIO)] = norm(enemyPossession->getCavalryNumber(), enemySafeDiv);
	inputData[idx(I::ENEMY_RANGE_RATIO)] = norm(enemyPossession->getRangeNumber(), enemySafeDiv);

	// Army counts (2)
	inputData[idx(I::ARMY_COUNT)] = norm(armyCount, 200.f);
	inputData[idx(I::ENEMY_ARMY_COUNT)] = norm(enemyArmyCount, 200.f);

	// Military strength (2)
	inputData[idx(I::ATTACK_SUM)] = norm(possession->getAttackSum(), 1000.f);
	inputData[idx(I::DEFENCE_ATTACK_SUM)] = norm(possession->getDefenceAttackSum(), 100.f);

	// Enemy score (1)
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), 1000.f);

	// Worker count — army vs worker balance (1)
	inputData[idx(I::WORKERS_COUNT)] = norm(possession->getWorkersNumber(), 100.f);

	// Urgencies from Master (2)
	inputData[idx(I::UNIT_URGENCY)] = unitUrgency;
	inputData[idx(I::ATTACK_URGENCY)] = attackUrgency;

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
