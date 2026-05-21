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
	// 14 inputs — no resource stockpiles (want list handles affordability)
	int idx = 0;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();

	// Our army composition — for counter-picking (3)
	float armyCount = static_cast<float>(possession->getArmyNumber());
	float safeDiv = std::max(armyCount, 1.f);
	inputData[idx++] = static_cast<float>(possession->getInfantryNumber()) / safeDiv;
	inputData[idx++] = static_cast<float>(possession->getCavalryNumber()) / safeDiv;
	inputData[idx++] = static_cast<float>(possession->getRangeNumber()) / safeDiv;

	// Enemy army composition — for counter-picking (3)
	float enemyArmyCount = static_cast<float>(enemyPossession->getArmyNumber());
	float enemySafeDiv = std::max(enemyArmyCount, 1.f);
	inputData[idx++] = static_cast<float>(enemyPossession->getInfantryNumber()) / enemySafeDiv;
	inputData[idx++] = static_cast<float>(enemyPossession->getCavalryNumber()) / enemySafeDiv;
	inputData[idx++] = static_cast<float>(enemyPossession->getRangeNumber()) / enemySafeDiv;

	// Army counts (2)
	inputData[idx++] = armyCount / 200.f;
	inputData[idx++] = enemyArmyCount / 200.f;

	// Military strength (2)
	inputData[idx++] = possession->getAttackSum() / 1000.f;
	inputData[idx++] = possession->getDefenceAttackSum() / 100.f;

	// Enemy score (1)
	inputData[idx++] = static_cast<float>(enemy->getScore()) / 1000.f;

	// Worker count — army vs worker balance (1)
	inputData[idx++] = static_cast<float>(possession->getWorkersNumber()) / 100.f;

	// Urgencies from Master (2)
	inputData[idx++] = unitUrgency;
	inputData[idx++] = attackUrgency;

	assert(idx == 14);

	auto result = brain->decide(std::span<const float>(inputData.data(), 14));

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
