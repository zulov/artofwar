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
	const auto plyMng = Game::getPlayersMan();
	auto* player = plyMng->getPlayer(playerId);
	const auto enemy = plyMng->getEnemyFor(playerId);

	METRIC_DEFINITIONS.writeResource(resourceIdInput,player, enemy);
	return resourceIdInput;
}

std::span<const float> AiInputProvider::getUnitsInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return writeBasicWith(unitsInput, player, player->getPossession()->getUnitsMetrics());
}

std::span<const float> AiInputProvider::getBuildingsInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return writeBasicWith(buildingsInput, player, player->getPossession()->getBuildingsMetrics());
}

std::span<const float> AiInputProvider::getUnitsInputWithMetric(char playerId, const db_unit_metric* prop) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	return writeBasicWith(unitsWithMetric, player, prop->getTypesVal());
}

std::span<const float> AiInputProvider::getBuildingsInputWithMetric(char playerId, const db_building_metric* prop,
                                                                    ParentBuildingType type) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	if (type == ParentBuildingType::RESOURCE) {
		return writeBasicWith(buildingsResWhereInput, player, prop->getValuesNormAsValForType(type));
	}
	return writeBasicWith(buildingsWhereInput, player, prop->getTypesVal());
}

std::span<const float> AiInputProvider::getAttackOrDefenceInput(char playerId) {
	const auto plyMng = Game::getPlayersMan();

	METRIC_DEFINITIONS.writeAttackOrDefence(attackOrDefenceInput,plyMng->getPlayer(playerId), plyMng->getEnemyFor(playerId));
	return attackOrDefenceInput;
}

std::span<const float> AiInputProvider::getWhereAttack(char playerId) {
	const auto plyMng = Game::getPlayersMan();
	const auto enemy = plyMng->getEnemyFor(playerId);
	auto* player = plyMng->getPlayer(playerId);

	METRIC_DEFINITIONS.writeWhereAttack(whereAttackInput,player,enemy);
	return whereAttackInput;
}

std::span<const float> AiInputProvider::getWhereDefend(char playerId) {
	const auto plyMng = Game::getPlayersMan();
	const auto enemy = plyMng->getEnemyFor(playerId);
	auto* player = plyMng->getPlayer(playerId);

	METRIC_DEFINITIONS.writeWhereDefend(whereDefendInput,player,enemy);
	return whereDefendInput;
}

std::span<const float> AiInputProvider::getBuildingsTypeInput(char playerId, ParentBuildingType type) {
	const auto player = Game::getPlayersMan()->getPlayer(playerId);
	const auto enemy = Game::getPlayersMan()->getEnemyFor(player->getId());
	const auto possession = player->getPossession();
	if (type == ParentBuildingType::RESOURCE) {
		 METRIC_DEFINITIONS.writeResourceWithOutBonus(buildingsResInput,player, enemy);
		 return buildingsResInput;
	}
	switch (type) {
	case ParentBuildingType::OTHER: return writeBasicWith(buildingsOtherInput, player, possession->getBuildingsMetrics(type));
	case ParentBuildingType::DEFENCE: return writeBasicWith(buildingsDefenceInput, player, possession->getBuildingsMetrics(type));
	case ParentBuildingType::RESOURCE: assert(false);
		return writeBasicWith(buildingsResInput, player, possession->getBuildingsMetrics(type));
	case ParentBuildingType::TECH: return writeBasicWith(buildingsTechInput, player,possession->getBuildingsMetrics(type));
	case ParentBuildingType::UNITS: return writeBasicWith(buildingsUnitsInput, player, possession->getBuildingsMetrics(type));
	default: ;
	}
}

template <std::size_t N>
std::span<float> AiInputProvider::writeBasic(std::array<float, N>& output, Player* player) {
	return METRIC_DEFINITIONS.writeBasic(output, player, Game::getPlayersMan()->getEnemyFor(player->getId()));
}

template <std::size_t N>
std::span<const float> AiInputProvider::writeBasicWith(std::array<float, N>& output, Player* player, std::span<const float> second) {
	auto result = METRIC_DEFINITIONS.writeBasic(output, player, Game::getPlayersMan()->getEnemyFor(player->getId()));
	std::copy(second.begin(), second.end(), result.begin());
	return output;
}
