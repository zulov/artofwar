#include "UnitBrain.h"

#include "AiUtils.h"
#include "NormScale.h"
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"

#include <algorithm>
#include <cmath>

UnitBrain::UnitBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->brainPrefix[3] + "unit.csv")),
	  nation(nation) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getInputSize() == magic_enum::enum_count<UnitInputIdx>());
	assert(brain->getOutputSize() == static_cast<int>(UnitOutputIdx::COUNT));
}

UnitOutput UnitBrain::decide(Player* player, Player* enemy,
                              float unitUrgency, float attackUrgency,
                              float preferInfantry, float preferRange, float preferCavalry,
                              float techUrgency, float gameTime) {
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
	inputData[idx(I::ARMY_COUNT)] = norm(armyCount, NormScale::ARMY);
	inputData[idx(I::ENEMY_ARMY_COUNT)] = norm(enemyArmyCount, NormScale::ARMY);

	// Military strength (2)
	inputData[idx(I::ATTACK_SUM)] = norm(possession->getAttackSum(), NormScale::ATTACK);
	inputData[idx(I::DEFENCE_ATTACK_SUM)] = norm(possession->getDefenceAttackSum(), NormScale::DEFENCE);

	// Enemy score (1)
	inputData[idx(I::ENEMY_SCORE)] = norm(enemy->getScore(), NormScale::SCORE);

	// Worker count — army vs worker balance (1)
	inputData[idx(I::WORKERS_COUNT)] = norm(possession->getWorkersNumber(), NormScale::WORKERS);

	// Urgencies from Master (2)
	inputData[idx(I::UNIT_URGENCY)] = unitUrgency;
	inputData[idx(I::ATTACK_URGENCY)] = attackUrgency;

	// Composition preferences from MilitaryBrain (3)
	inputData[idx(I::PREFER_INFANTRY)] = preferInfantry;
	inputData[idx(I::PREFER_RANGE)] = preferRange;
	inputData[idx(I::PREFER_CAVALRY)] = preferCavalry;

	// Upgrade inputs (4)
	inputData[idx(I::TECH_URGENCY)] = techUrgency;

	// Average unit level across nation's non-worker units
	inputData[idx(I::AVG_UNIT_LEVEL)] = avgUnitLevel(nation->units, player,
		[](const db_unit* u) { return !u->typeWorker; });

	// Average building level across nation's unit-producing buildings
	inputData[idx(I::AVG_BUILDING_LEVEL)] = avgBuildingLevel(nation->buildings, player, ParentBuildingType::UNITS);

	inputData[idx(I::GAME_TIME)] = gameTime;

	auto result = brain->decide(inputData);

	UnitOutput output{};

	// Unit profile (23-dim) — reused for both production and upgrade matching
	std::copy_n(result.begin(), UNIT_PROFILE_SIZE, output.unitProfile.begin());

	// Upgrade urgencies
	output.unitUpgradeUrgency = result[static_cast<int>(UnitOutputIdx::UNIT_UPGRADE_URGENCY)];
	output.buildingUpgradeUrgency = result[static_cast<int>(UnitOutputIdx::BUILDING_UPGRADE_URGENCY)];

	// Count scaling
	if (unitUrgency > 0.f) {
		int raw = roundToInt(unitUrgency * MAX_UNITS_PER_TICK);
		output.count = static_cast<unsigned char>(std::max(1, raw));
	} else {
		output.count = 0;
	}

	return output;
}
