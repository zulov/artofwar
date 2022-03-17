#include "AiInputProvider.h"

#include "Game.h"
#include "math/SpanUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiMetric.h"
#include "player/ai/AiUtils.h"
#include "objects/building/ParentBuildingType.h"


AiInputProvider::AiInputProvider() {
	// std::cout << std::format("AI Sizes INPUT\t Res: {}, Unit: {}, Build: {}, UnitM: {}, BuildM: {}\n",
	// 	resourceIdInputSpan.size(), unitsInputSpan.size(), buildingsInputSpan.size(), unitsWithMetricUnitSpan.size(), basicWithMetricUnitSpan.size());
}

std::span<float> AiInputProvider::getResourceInput(char playerId) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(resourceIdInputSpan,
	                        METRIC_DEFINITIONS.getResourceNorm(player->getResources(), player->getPossession()),
	                        player);
}

std::span<float> AiInputProvider::getUnitsInput(char playerId) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(unitsInputSpan, player->getPossession().getUnitsMetrics(), player);
}

std::span<float> AiInputProvider::getBuildingsInput(char playerId) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(buildingsInputSpan, player->getPossession().getBuildingsMetrics(), player);
}

std::span<float> AiInputProvider::getUnitsInputWithMetric(char playerId, const db_unit_metric* prop) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(unitsWithMetricUnitSpan, prop->getTypesVal(), player);
}

std::span<float> AiInputProvider::getBuildingsInputWithMetric(char playerId, const db_building_metric* prop,
	ParentBuildingType type) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return combineWithBasic(buildingsWhereInputSpan, prop->getTypesVal(), player);
}

// std::span<float> AiInputProvider::getBuildingsInputWithMetric(char playerId, const db_building_metric* prop, ParentBuildingType type) const {
// 	auto* player = Game::getPlayersMan()->getPlayer(playerId);
//
// 	return combineWithBasic(basicWithMetricUnitSpan, prop->getValuesNormSmall(), player);
// }

std::span<float> AiInputProvider::getAttackOrDefenceInput(char playerId) const {
	const auto plyMng = Game::getPlayersMan();
	const char idEnemy = plyMng->getEnemyFor(playerId);

	auto& data = METRIC_DEFINITIONS.getAttackOrDefenceNorm(plyMng->getPlayer(playerId), plyMng->getPlayer(idEnemy));
	std::ranges::copy(data, attackOfDefenceInputSpan.begin());

	assert(validateSpan(__LINE__, __FILE__, attackOfDefenceInputSpan));
	return attackOfDefenceInputSpan;
}

std::span<float> AiInputProvider::getWhereAttack(char playerId) const {
	const auto plyMng = Game::getPlayersMan();
	auto* player = plyMng->getPlayer(playerId);
	const char idEnemy = plyMng->getEnemyFor(playerId);

	return combineWithBasic(whereAttackInputSpan,
	                        METRIC_DEFINITIONS.getWhereAttackNorm(player, plyMng->getPlayer(idEnemy)), player);
}

std::span<float> AiInputProvider::getWhereDefend(char playerId) const {
	const auto plyMng = Game::getPlayersMan();
	auto* player = plyMng->getPlayer(playerId);
	const char idEnemy = plyMng->getEnemyFor(playerId);

	return combineWithBasic(whereDefendInputSpan,
	                        METRIC_DEFINITIONS.getWhereDefendNorm(player, plyMng->getPlayer(idEnemy)), player);
}

std::span<float> AiInputProvider::getBuildingInputSpan(ParentBuildingType type) const {
	switch (type) {
	case ParentBuildingType::OTHER: return buildingsOtherInputSpan;
	case ParentBuildingType::DEFENCE: return buildingsDefenceInputSpan;
	case ParentBuildingType::RESOURCE: return buildingsResInputSpan;
	case ParentBuildingType::TECH: return buildingsTechInputSpan;
	case ParentBuildingType::UNITS: return buildingsUnitsInputSpan;
	default: ;
	}
}

std::span<float> AiInputProvider::getBuildingsTypeInput(char playerId, ParentBuildingType type) const {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	const std::span<float> data = player->getPossession().getBuildingsMetrics(type);

	return combineWithBasic(getBuildingInputSpan(type), data, player);
}

std::span<float> AiInputProvider::getBasicInput(std::span<float> dest, Player* player) const {
	const auto plyMng = Game::getPlayersMan();
	char idEnemy = plyMng->getEnemyFor(player->getId());
	auto& data = METRIC_DEFINITIONS.getBasicNorm(player, plyMng->getPlayer(idEnemy));
	assert(validateSpan(__LINE__, __FILE__, data));
	std::ranges::copy(data, dest.begin());

	return std::span(dest.begin() + data.size(), dest.size() - data.size());
}

std::span<float> AiInputProvider::combineWithBasic(const std::span<float> output, const std::span<const float> toJoin,
                                                   Player* player) const {
	std::ranges::copy(toJoin, getBasicInput(output, player).begin());

	assert(validateSpan(__LINE__, __FILE__, output));
	return output;
}

std::span<float> AiInputProvider::combineWithBasic(const std::span<float> output, const std::vector<float>& toJoin,
                                                   Player* player) const {
	std::ranges::copy(toJoin, getBasicInput(output, player).begin());

	assert(validateSpan(__LINE__, __FILE__, output));
	return output;
}
