#include "MilitaryBrain.h"

#include <magic_enum.hpp>
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
	assert(brain->getOutputSize() == magic_enum::enum_count<MilitaryOutputIdx>());
}

MilitaryOutput MilitaryBrain::decide(Player* player, Player* enemy,
                                      float militaryUrgency, float attackUrgency) {
	// 14 inputs — army orders are free, no resource info needed
	int idx = 0;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();

	// Military strength (2)
	inputData[idx++] = possession->getAttackSum() / 1000.f;
	inputData[idx++] = possession->getDefenceAttackSum() / 100.f;

	// Spatial — army/building distances (4)
	inputData[idx++] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, player, 0.f);
	inputData[idx++] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, enemy, 1.f);
	inputData[idx++] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);
	inputData[idx++] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, enemy, 0.f);

	// Scores (2)
	inputData[idx++] = enemy->getScore() / 1000.f;
	inputData[idx++] = player->getScore() / 1000.f;

	// Unit & army counts (4)
	inputData[idx++] = static_cast<float>(possession->getUnitsNumber()) / 200.f;
	inputData[idx++] = static_cast<float>(enemyPossession->getUnitsNumber()) / 200.f;
	inputData[idx++] = static_cast<float>(possession->getArmyNumber()) / 200.f;
	inputData[idx++] = static_cast<float>(possession->getFreeArmyNumber()) / 200.f;

	// Urgencies from Master (2)
	inputData[idx++] = militaryUrgency;
	inputData[idx++] = attackUrgency;

	assert(idx == 14);

	auto result = brain->decide(std::span<const float>(inputData.data(), 14));

	return MilitaryOutput{
		result[0], result[1], result[2], result[3]
	};
}
