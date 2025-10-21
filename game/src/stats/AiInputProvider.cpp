#include "AiInputProvider.h"

#include "Game.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiUtils.h"
#include "objects/building/ParentBuildingType.h"
#include "player/Possession.h"
#include "player/ai/MetricDefinitions.h"
#include "player/ai/PossessionMetric.h"


std::span<const float> AiInputProvider::getResourceInput(Player* player, Player* enemy) {
	return METRIC_DEFINITIONS.writeResource(player, enemy);
}

std::span<const float> AiInputProvider::getUnitsInput(Player* player) {
	return basicWithSpan(player, player->getPossession()->getMetrics()->unitsSum);
}

std::span<const float> AiInputProvider::getBuildingsInput(Player* player) {
	return basicWithSpan(player, player->getPossession()->getMetrics()->buildingsSum);
}

std::span<const float> AiInputProvider::getUnitsInputWithMetric(Player* player, const db_unit_metric* prop) {
	return basicWithSpan(player, prop->getTypesVal());
}

std::span<const float> AiInputProvider::getBuildingsInputWithMetric(Player* player, const db_building_metric* prop,
                                                                    ParentBuildingType type) {
	return basicWithSpan(player, type == ParentBuildingType::RESOURCE
		                             ? prop->getValuesNormAsValForType(type)
		                             : prop->getTypesVal());
}

std::span<const float> AiInputProvider::getAttackOrDefenceInput(Player* player, Player* enemy) {
	return METRIC_DEFINITIONS.writeAttackOrDefence(player, enemy);
}

std::span<const float> AiInputProvider::getWhereAttack(Player* player, Player* enemy) {
	return METRIC_DEFINITIONS.writeWhereAttack(player, enemy);
}

std::span<const float> AiInputProvider::getWhereDefend(Player* player, Player* enemy) {
	return METRIC_DEFINITIONS.writeWhereDefend(player, enemy);
}

std::span<const float> AiInputProvider::getBuildingsTypeInput(Player* player, ParentBuildingType type) {
	const auto enemy = Game::getPlayersMan()->getEnemyFor(player->getId());
	switch (type) {
	case ParentBuildingType::RESOURCE:
		return METRIC_DEFINITIONS.writeResourceWithOutBonus(player, enemy);
	case ParentBuildingType::OTHER:
		return basicWithSpanSelective(player, enemy, METRIC_DEFINITIONS.getBuildingOtherIdxs());
	case ParentBuildingType::DEFENCE:
		return basicWithSpanSelective(player, enemy, METRIC_DEFINITIONS.getBuildingDefenceIdxs());
	case ParentBuildingType::TECH:
		return basicWithSpanSelective(player, enemy, METRIC_DEFINITIONS.getBuildingTechIdxs());
	case ParentBuildingType::UNITS:
		return basicWithSpanSelective(player, enemy, METRIC_DEFINITIONS.getBuildingUnitsIdxs());
	}
}

std::span<const float> AiInputProvider::basicWithSpan(Player* player, std::span<const float> span) {
	return METRIC_DEFINITIONS.basicWithSpan(player, Game::getPlayersMan()->getEnemyFor(player->getId()), span);
}

std::span<const float> AiInputProvider::basicWithSpanSelective(Player* player, Player* enemy, std::span<const unsigned char> idxs) {
	return METRIC_DEFINITIONS.basicWithSpanSelective(player, enemy, player->getPossession()->getMetrics()->buildingsSum,
	                                                 idxs);
}
