#include "AiInputProvider.h"

#include "Game.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "objects/building/ParentBuildingType.h"
#include "player/Possession.h"
#include "player/ai/MetricDefinitions.h"
#include "player/ai/PossessionMetric.h"


std::span<const float> AiInputProvider::forResource(Player* player, Player* enemy) {
	return METRIC_DEFINITIONS.writeResource(player, enemy);
}

std::span<const float> AiInputProvider::forUnits(Player* player) {
	return basicWithSpan(player, player->getPossession()->getMetrics()->unitsSum);
}

std::span<const float> AiInputProvider::forBuildings(Player* player) {
	return basicWithSpan(player, player->getPossession()->getMetrics()->buildingsSum);
}

std::span<const float> AiInputProvider::forUnitPlacement(Player* player, const db_unit_metric* prop) {
	return basicWithSpan(player, prop->getTypesVal());
}

std::span<const float> AiInputProvider::forBuildingPlacement(Player* player, const db_building_metric* prop,
                                                                    ParentBuildingType type) {
	return basicWithSpan(player, type == ParentBuildingType::RESOURCE
		                             ? prop->getValuesNormAsValForType(type)
		                             : prop->getTypesVal());
}

std::span<const float> AiInputProvider::forAttackOrDefence(Player* player, Player* enemy) {
	return METRIC_DEFINITIONS.writeAttackOrDefence(player, enemy);
}

std::span<const float> AiInputProvider::forWhereAttack(Player* player, Player* enemy) {
	return METRIC_DEFINITIONS.writeWhereAttack(player, enemy);
}

std::span<const float> AiInputProvider::forWhereDefend(Player* player, Player* enemy) {
	return METRIC_DEFINITIONS.writeWhereDefend(player, enemy);
}

std::span<const float> AiInputProvider::forBuildingType(Player* player, ParentBuildingType type) {
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
	return METRIC_DEFINITIONS.compose(
		section(METRIC_DEFINITIONS.aiBasicMetric, player, Game::getPlayersMan()->getEnemyFor(player->getId())),
		section(span)
	);
}

std::span<const float> AiInputProvider::basicWithSpanSelective(Player* player, Player* enemy, std::span<const unsigned char> idxs) {
	return METRIC_DEFINITIONS.compose(
		section(METRIC_DEFINITIONS.aiBasicMetric, player, enemy),
		section(player->getPossession()->getMetrics()->buildingsSum, idxs)
	);
}
