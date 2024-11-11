#include "AiInputProvider.h"

#include "Game.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiUtils.h"
#include "objects/building/ParentBuildingType.h"
#include "player/Possession.h"
#include "player/ai/MetricDefinitions.h"


std::span<const float> AiInputProvider::getResourceInput(Player* player, Player* enemy) {
	METRIC_DEFINITIONS.writeResource(resourceIdInput,player, enemy);
	return resourceIdInput;
}

std::span<const float> AiInputProvider::getUnitsInput(Player* player) {
	return writeBasicWith(unitsInput, player, player->getPossession()->getUnitsMetrics());
}

std::span<const float> AiInputProvider::getBuildingsInput(Player* player) {
	return writeBasicWith(buildingsInput, player, player->getPossession()->getBuildingsMetrics());
}

std::span<const float> AiInputProvider::getUnitsInputWithMetric(Player* player, const db_unit_metric* prop) {
	return writeBasicWith(unitsWithMetric, player, prop->getTypesVal());
}

std::span<const float> AiInputProvider::getBuildingsInputWithMetric(Player* player, const db_building_metric* prop,
                                                                    ParentBuildingType type) {
	if (type == ParentBuildingType::RESOURCE) {
		return writeBasicWith(buildingsResWhereInput, player, prop->getValuesNormAsValForType(type));
	}
	return writeBasicWith(buildingsWhereInput, player, prop->getTypesVal());
}

std::span<const float> AiInputProvider::getAttackOrDefenceInput(Player* player, Player* enemy) {
	METRIC_DEFINITIONS.writeAttackOrDefence(attackOrDefenceInput, player, enemy);
	return attackOrDefenceInput;
}

std::span<const float> AiInputProvider::getWhereAttack(Player* player, Player* enemy) {
	METRIC_DEFINITIONS.writeWhereAttack(whereAttackInput, player, enemy);
	return whereAttackInput;
}

std::span<const float> AiInputProvider::getWhereDefend(Player* player, Player* enemy) {
	METRIC_DEFINITIONS.writeWhereDefend(whereDefendInput, player, enemy);
	return whereDefendInput;
}

std::span<const float> AiInputProvider::getBuildingsTypeInput(Player* player, Player* enemy, ParentBuildingType type) {
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
