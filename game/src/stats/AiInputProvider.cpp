#include "AiInputProvider.h"

#include "Game.h"
#include "math/SpanUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiUtils.h"
#include "objects/building/ParentBuildingType.h"
#include "player/Possession.h"
#include "player/ai/MetricDefinitions.h"


AiInputProvider::AiInputProvider() {
	// std::cout << std::format("AI Sizes INPUT\t Res: {}, Unit: {}, Build: {}, UnitM: {}, BuildM: {}\n",
	// 	resourceIdInputSpan.size(), unitsInputSpan.size(), buildingsInputSpan.size(), unitsWithMetricUnitSpan.size(), basicWithMetricUnitSpan.size());
}

std::span<float> AiInputProvider::getResourceInput(char playerId) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(resourceIdInputSpan,
	                        METRIC_DEFINITIONS.getResourceNorm(player->getResources(), player->getPossession(),
	                                                           METRIC_DEFINITIONS.aiResInputIdxs), player);
}

std::span<float> AiInputProvider::getUnitsInput(char playerId) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(unitsInputSpan, player->getPossession()->getUnitsMetrics(), player);
}

std::span<float> AiInputProvider::getBuildingsInput(char playerId) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(buildingsInputSpan, player->getPossession()->getBuildingsMetrics(), player);
}

std::span<float> AiInputProvider::getUnitsInputWithMetric(char playerId, const db_unit_metric* prop) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(unitsWithMetricUnitSpan, prop->getTypesVal(), player);
}

std::span<float> AiInputProvider::getBuildingsInputWithMetric(char playerId, const db_building_metric* prop,
                                                              ParentBuildingType type) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	if (type == ParentBuildingType::RESOURCE) {
		return combineWithBasic(buildingsResWhereInputSpan, prop->getValuesNormAsValForType(type), player);
	}
	return combineWithBasic(buildingsWhereInputSpan, prop->getTypesVal(), player);
}

std::span<float> AiInputProvider::getAttackOrDefenceInput(char playerId) const {
	const auto plyMng = Game::getPlayersMan();

	auto& data = METRIC_DEFINITIONS.getAttackOrDefenceNorm(plyMng->getPlayer(playerId), plyMng->getEnemyFor(playerId));
	std::ranges::copy(data, attackOfDefenceInputSpan.begin());

	assert(validateSpan(__LINE__, __FILE__, attackOfDefenceInputSpan));
	return attackOfDefenceInputSpan;
}

std::span<float> AiInputProvider::getWhereAttack(char playerId) const {
	const auto plyMng = Game::getPlayersMan();
	auto* player = plyMng->getPlayer(playerId);

	return combineWithBasic(whereAttackInputSpan,
	                        METRIC_DEFINITIONS.getWhereAttackNorm(player, plyMng->getEnemyFor(playerId)), player);
}

std::span<float> AiInputProvider::getWhereDefend(char playerId) const {
	const auto plyMng = Game::getPlayersMan();
	auto* player = plyMng->getPlayer(playerId);

	return combineWithBasic(whereDefendInputSpan,
	                        METRIC_DEFINITIONS.getWhereDefendNorm(player, plyMng->getEnemyFor(playerId)), player);
}

std::span<float> AiInputProvider::getBuildingInputSpan(ParentBuildingType type) const {
	switch (type) {
	case ParentBuildingType::OTHER: return buildingsOtherInputSpan;
	case ParentBuildingType::DEFENCE: return buildingsDefenceInputSpan;
	case ParentBuildingType::RESOURCE: assert(false);
		return buildingsResInputSpan;
	case ParentBuildingType::TECH: return buildingsTechInputSpan;
	case ParentBuildingType::UNITS: return buildingsUnitsInputSpan;
	default: ;
	}
}

std::span<float> AiInputProvider::getBuildingsTypeInput(char playerId, ParentBuildingType type) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	if (type == ParentBuildingType::RESOURCE) {
		return combineWithBasic(buildingsResInputSpan,
		                        METRIC_DEFINITIONS.getResourceNorm(player->getResources(), player->getPossession(),
		                                                           METRIC_DEFINITIONS.aiResWithoutBonusIdxsSpan), player);
	}
	return combineWithBasic(getBuildingInputSpan(type), player->getPossession()->getBuildingsMetrics(type), player);
}

std::span<float> AiInputProvider::getBasicInput(std::span<float> dest, Player* player) const {
	const auto plyMng = Game::getPlayersMan();

	auto& data = METRIC_DEFINITIONS.getBasicNorm(player, plyMng->getEnemyFor(player->getId()));
	assert(validateSpan(__LINE__, __FILE__, data));
	std::ranges::copy(data, dest.begin());

	return std::span(dest.begin() + data.size(), dest.size() - data.size());
}

std::span<float> AiInputProvider::combineWithBasic(const std::span<float> output, const std::span<const float> toJoin,
                                                   Player* player) const {
	std::ranges::copy(toJoin, getBasicInput(output, player).begin());
	assert(toJoin.size() > 0);
	assert(validateSpan(__LINE__, __FILE__, toJoin));
	assert(validateSpan(__LINE__, __FILE__, output));
	assert(output.size() == BASIC_SIZE + toJoin.size());
	return output;
}

std::span<float> AiInputProvider::combineWithBasic(const std::span<float> output, const std::vector<float>& toJoin,
                                                   Player* player) const {
	std::ranges::copy(toJoin, getBasicInput(output, player).begin());
	assert(toJoin.size() > 0);
	assert(validateSpan(__LINE__, __FILE__, toJoin));
	assert(validateSpan(__LINE__, __FILE__, output));
	assert(output.size() == BASIC_SIZE + toJoin.size());
	return output;
}
