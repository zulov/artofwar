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

std::span<const float> AiInputProvider::getResourceInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(resourceIdInputArray,
	                        METRIC_DEFINITIONS.getResourceNorm(player->getResources(), player->getPossession(),
	                                                          METRIC_DEFINITIONS.aiResInputIdxs), player);
}

std::span<const float> AiInputProvider::getUnitsInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(unitsInput, player->getPossession()->getUnitsMetrics(), player);
}

std::span<const float> AiInputProvider::getBuildingsInput(char playerId){
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(buildingsInput, player->getPossession()->getBuildingsMetrics(), player);
}

std::span<const float> AiInputProvider::getUnitsInputWithMetric(char playerId, const db_unit_metric* prop) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(unitsWithMetric, prop->getTypesVal(), player);
}

std::span<const float> AiInputProvider::getBuildingsInputWithMetric(char playerId, const db_building_metric* prop,
                                                              ParentBuildingType type) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	if (type == ParentBuildingType::RESOURCE) {
		return combineWithBasic(buildingsResWhereInput, prop->getValuesNormAsValForType(type), player);
	}
	return combineWithBasic(buildingsWhereInput, prop->getTypesVal(), player);
}

std::span<const float> AiInputProvider::getAttackOrDefenceInput(char playerId) {
	const auto plyMng = Game::getPlayersMan();

	auto& data = METRIC_DEFINITIONS.getAttackOrDefenceNorm(plyMng->getPlayer(playerId), plyMng->getEnemyFor(playerId));
	//std::ranges::copy(data, std::begin(attackOrDefenceInput));
	std::copy(data.begin(), data.end(), attackOrDefenceInput.begin());

	assert(validateSpan(__LINE__, __FILE__, attackOrDefenceInput));
	return asSpan3(attackOrDefenceInput);
}

std::span<const float> AiInputProvider::getWhereAttack(char playerId) {
	const auto plyMng = Game::getPlayersMan();
	auto* player = plyMng->getPlayer(playerId);

	return combineWithBasic(whereAttackInput,
	                        METRIC_DEFINITIONS.getWhereAttackNorm(player, plyMng->getEnemyFor(playerId)), player);
}

std::span<const float> AiInputProvider::getWhereDefend(char playerId) {
	const auto plyMng = Game::getPlayersMan();
	auto* player = plyMng->getPlayer(playerId);

	return combineWithBasic(whereDefendInput,
	                        METRIC_DEFINITIONS.getWhereDefendNorm(player, plyMng->getEnemyFor(playerId)), player);
}

std::span<const float> AiInputProvider::getBuildingsTypeInput(char playerId, ParentBuildingType type) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	if (type == ParentBuildingType::RESOURCE) {
		return combineWithBasic(buildingsResInput,
		                        METRIC_DEFINITIONS.getResourceNorm(player->getResources(), player->getPossession(),
		                                                           METRIC_DEFINITIONS.aiResWithoutBonusIdxs), player);
	}
	switch (type) {
	case ParentBuildingType::OTHER: return combineWithBasic(buildingsOtherInput, player->getPossession()->getBuildingsMetrics(type), player);
	case ParentBuildingType::DEFENCE: return combineWithBasic(buildingsDefenceInput, player->getPossession()->getBuildingsMetrics(type), player);
	case ParentBuildingType::RESOURCE: assert(false);
		return combineWithBasic(buildingsResInput, player->getPossession()->getBuildingsMetrics(type), player);
	case ParentBuildingType::TECH: return combineWithBasic(buildingsTechInput, player->getPossession()->getBuildingsMetrics(type), player);
	case ParentBuildingType::UNITS: return combineWithBasic(buildingsUnitsInput, player->getPossession()->getBuildingsMetrics(type), player);
	default:;
	}
}

template <std::size_t N>
std::span<const float> AiInputProvider::combineWithBasic(std::array<float, N>& output, std::span<const float> toJoin,
                                                   Player* player) {
	auto& data = METRIC_DEFINITIONS.getBasicNorm(player, Game::getPlayersMan()->getEnemyFor(player->getId()));
	std::copy(data.begin(), data.end(), output.begin());
	std::copy(toJoin.begin(), toJoin.end(), output.begin()+data.size());
	//std::ranges::copy(toJoin, getBasicInput(output, player).begin());
	assert(toJoin.size() > 0);
	assert(validateSpan(__LINE__, __FILE__, toJoin));
	assert(validateSpan(__LINE__, __FILE__, output));
	assert(output.size() == BASIC_SIZE + toJoin.size());
	return asSpan2(output);
}
