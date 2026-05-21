#include "MasterBrain.h"

#include <magic_enum.hpp>
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "MetricDefinitions.h"
#include "database/db_struct.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"
#include "player/PlayersManager.h"
#include "Game.h"
#include "stats/AiInputProvider.h"
#include "env/influence/CenterType.h"

MasterBrain::MasterBrain(db_nation* nation)
	: brain(BrainProvider::get(nation->actionPrefix[0] + "master.csv")) {
	assert(brain->getInputSize() == inputData.size());
	assert(brain->getOutputSize() == magic_enum::enum_count<MasterOutputIdx>());
}

MasterOutput MasterBrain::decide(Player* player, Player* enemy, std::span<const float> lackingPerResource, float totalLacking) {
	float deltaScore = player->getScore() - prevScore;
	float deltaUnits = static_cast<float>(player->getPossession()->getUnitsNumber()) - prevUnits;
	float resSum = 0.f;
	for (int i = 0; i < 4; ++i) {
		resSum += player->getResources()->getValues()[i];
	}
	float deltaRes = resSum - prevResSum;

	// Normalize deltas
	deltaScore /= 1000.f;
	deltaUnits /= 200.f;
	deltaRes /= 1000.f;

	// Normalize lacking
	std::array<float, 4> normLacking{};
	for (int i = 0; i < 4; ++i) {
		normLacking[i] = lackingPerResource.size() > static_cast<size_t>(i) ? lackingPerResource[i] / 500.f : 0.f;
	}
	float normTotalLacking = totalLacking / 2000.f;

	// Build input array — 29 inputs
	int idx = 0;

	auto* possession = player->getPossession();
	auto* enemyPossession = enemy->getPossession();
	auto* res = player->getResources();

	// Scores (2)
	inputData[idx++] = player->getScore() / 1000.f;
	inputData[idx++] = enemy->getScore() / 1000.f;

	// Counts (4)
	inputData[idx++] = static_cast<float>(possession->getBuildingsNumber()) / 50.f;
	inputData[idx++] = static_cast<float>(possession->getWorkersNumber()) / 100.f;
	inputData[idx++] = static_cast<float>(possession->getArmyNumber()) / 200.f;
	inputData[idx++] = static_cast<float>(enemyPossession->getArmyNumber()) / 200.f;

	// Resources — stockpiles (4)
	inputData[idx++] = res->getValue(ResourceType::FOOD) / 1000.f;
	inputData[idx++] = res->getValue(ResourceType::WOOD) / 1000.f;
	inputData[idx++] = res->getValue(ResourceType::STONE) / 1000.f;
	inputData[idx++] = res->getValue(ResourceType::GOLD) / 1000.f;

	// Resources — gather speeds (4)
	inputData[idx++] = res->getGatherSpeed(ResourceType::FOOD) / 10.f;
	inputData[idx++] = res->getGatherSpeed(ResourceType::WOOD) / 10.f;
	inputData[idx++] = res->getGatherSpeed(ResourceType::STONE) / 10.f;
	inputData[idx++] = res->getGatherSpeed(ResourceType::GOLD) / 10.f;

	// Workers (1)
	inputData[idx++] = static_cast<float>(possession->getFreeWorkersNumber()) / 100.f;

	// Attack/Defence (2)
	inputData[idx++] = possession->getAttackSum() / 1000.f;
	inputData[idx++] = possession->getDefenceAttackSum() / 100.f;

	// Spatial distances (4)
	inputData[idx++] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, player, 0.f);
	inputData[idx++] = MetricDefinitions::diffOfCenters(CenterType::ARMY, player, CenterType::BUILDING, enemy, 1.f);
	inputData[idx++] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, player, 1.f);
	inputData[idx++] = MetricDefinitions::diffOfCenters(CenterType::ARMY, enemy, CenterType::BUILDING, enemy, 0.f);

	// Lacking feedback (4 + 1)
	for (int i = 0; i < 4; ++i) {
		inputData[idx++] = normLacking[i];
	}
	inputData[idx++] = normTotalLacking;

	// Deltas (3)
	inputData[idx++] = deltaScore;
	inputData[idx++] = deltaUnits;
	inputData[idx++] = deltaRes;

	assert(idx == 29);

	updateHistory(player);

	auto result = brain->decide(std::span<const float>(inputData.data(), 29));

	return MasterOutput{
		result[0], result[1], result[2], result[3],
		result[4], result[5], result[6], result[7], result[8]
	};
}

void MasterBrain::updateHistory(Player* player) {
	prevScore = player->getScore();
	prevUnits = static_cast<float>(player->getPossession()->getUnitsNumber());
	prevResSum = 0.f;
	for (int i = 0; i < 4; ++i) {
		prevResSum += player->getResources()->getValues()[i];
	}
}
