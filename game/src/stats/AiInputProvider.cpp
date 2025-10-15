#include "AiInputProvider.h"

#include "Game.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiUtils.h"
#include "objects/building/ParentBuildingType.h"
#include "player/Possession.h"
#include "player/ai/MetricDefinitions.h"


std::span<const float> AiInputProvider::getResourceInput(Player* player, Player* enemy) {
	return METRIC_DEFINITIONS.writeResource(player, enemy);
}

std::span<const float> AiInputProvider::getUnitsInput(Player* player) {
	return basicWithSpan(player, player->getPossession()->getUnitsMetrics());
}

std::span<const float> AiInputProvider::getBuildingsInput(Player* player) {
	return basicWithSpan(player, player->getPossession()->getBuildingsMetrics());
}

std::span<const float> AiInputProvider::getUnitsInputWithMetric(Player* player, const db_unit_metric* prop) {
	return basicWithSpan(player, prop->getTypesVal());
}

std::span<const float> AiInputProvider::getBuildingsInputWithMetric(Player* player, const db_building_metric* prop,
                                                                    ParentBuildingType type) {
	return basicWithSpan(player, type == ParentBuildingType::RESOURCE ? prop->getValuesNormAsValForType(type) : prop->getTypesVal());
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

std::span<const float> AiInputProvider::getBuildingsTypeInput(Player* player, Player* enemy, ParentBuildingType type) {
	switch (type) {
	case ParentBuildingType::RESOURCE: return METRIC_DEFINITIONS.writeResourceWithOutBonus(player, enemy);
	case ParentBuildingType::OTHER:
	case ParentBuildingType::DEFENCE:
	case ParentBuildingType::TECH:
	case ParentBuildingType::UNITS:
		return basicWithSpan(player, player->getPossession()->getBuildingsMetrics(type));
	}
}

std::span<const float> AiInputProvider::basicWithSpan(Player* player, std::span<const float> span) {
	return METRIC_DEFINITIONS.basicWithSpan(player, Game::getPlayersMan()->getEnemyFor(player->getId()), span);
}
