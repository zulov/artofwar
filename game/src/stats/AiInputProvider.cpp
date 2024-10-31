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

	METRIC_DEFINITIONS.writeResource(
		writeBasic(resourceIdInput, player),
		player->getResources(), player->getPossession());
	assert(validateSpan(__LINE__, __FILE__, resourceIdInput));
	return resourceIdInput;
}

std::span<const float> AiInputProvider::getUnitsInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return writeBasic(unitsInput, player->getPossession()->getUnitsMetrics(), player);
	return unitsInput;
}

std::span<const float> AiInputProvider::getBuildingsInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return writeBasic(buildingsInput, player->getPossession()->getBuildingsMetrics(), player);
	return buildingsInput;
}

std::span<const float> AiInputProvider::getUnitsInputWithMetric(char playerId, const db_unit_metric* prop) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return writeBasic(unitsWithMetric, prop->getTypesVal(), player);
	return unitsWithMetric;
}

std::span<const float> AiInputProvider::getBuildingsInputWithMetric(char playerId, const db_building_metric* prop,
                                                                    ParentBuildingType type) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	if (type == ParentBuildingType::RESOURCE) {
		writeBasic(buildingsResWhereInput, prop->getValuesNormAsValForType(type), player);
		return buildingsResWhereInput;
	}
	writeBasic(buildingsWhereInput, prop->getTypesVal(), player);
	return buildingsWhereInput;
}

std::span<const float> AiInputProvider::getAttackOrDefenceInput(char playerId) {
	const auto plyMng = Game::getPlayersMan();

	METRIC_DEFINITIONS.writeAttackOrDefence(attackOrDefenceInput,plyMng->getPlayer(playerId), plyMng->getEnemyFor(playerId));

	assert(validateSpan(__LINE__, __FILE__, attackOrDefenceInput));
	return attackOrDefenceInput;
}

std::span<const float> AiInputProvider::getWhereAttack(char playerId) {
	const auto plyMng = Game::getPlayersMan();
	auto* player = plyMng->getPlayer(playerId);

	METRIC_DEFINITIONS.writeWhereAttack(
		writeBasic(whereAttackInput, player),
		player, plyMng->getEnemyFor(playerId));
	assert(validateSpan(__LINE__, __FILE__, whereAttackInput));

	//
	// return writeBasic(whereAttackInput,
	//                   METRIC_DEFINITIONS.writeWhereAttack(player, plyMng->getEnemyFor(playerId)), player);
	return whereAttackInput;
}

std::span<const float> AiInputProvider::getWhereDefend(char playerId) {
	const auto plyMng = Game::getPlayersMan();
	auto* player = plyMng->getPlayer(playerId);

	METRIC_DEFINITIONS.writeWhereDefend(
		writeBasic(whereDefendInput, player),
		player, plyMng->getEnemyFor(playerId));
	assert(validateSpan(__LINE__, __FILE__, whereDefendInput));
	return whereDefendInput;
}

std::span<const float> AiInputProvider::getBuildingsTypeInput(char playerId, ParentBuildingType type) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	if (type == ParentBuildingType::RESOURCE) {
		 METRIC_DEFINITIONS.writeResourceWithOutBonus(
			 writeBasic(buildingsResInput, player), 
			 player->getResources(), player->getPossession());
		 return buildingsResInput;
	}
	switch (type) {
	case ParentBuildingType::OTHER: return writeBasic(buildingsOtherInput,
	                                                  player->getPossession()->getBuildingsMetrics(type), player);
	case ParentBuildingType::DEFENCE: return writeBasic(buildingsDefenceInput,
	                                                    player->getPossession()->getBuildingsMetrics(type), player);
	case ParentBuildingType::RESOURCE: assert(false);
		return writeBasic(buildingsResInput, player->getPossession()->getBuildingsMetrics(type), player);
	case ParentBuildingType::TECH: return writeBasic(buildingsTechInput,
	                                                 player->getPossession()->getBuildingsMetrics(type), player);
	case ParentBuildingType::UNITS: return writeBasic(buildingsUnitsInput,
	                                                  player->getPossession()->getBuildingsMetrics(type), player);
	default: ;
	}
}

template <std::size_t N>
std::span<float> AiInputProvider::writeBasic(std::array<float, N>& output, Player* player) {
	return METRIC_DEFINITIONS.writeBasic(output, player, Game::getPlayersMan()->getEnemyFor(player->getId()));
}
