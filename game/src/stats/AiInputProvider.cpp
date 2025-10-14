#include "AiInputProvider.h"

#include "Game.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiUtils.h"
#include "objects/building/ParentBuildingType.h"
#include "player/Possession.h"
#include "player/ai/MetricDefinitions.h"


std::span<const float> AiInputProvider::getResourceInput(Player* player, Player* enemy) {
	METRIC_DEFINITIONS.writeResource(input, player, enemy);
	return resourceIdInput;
}

std::span<const float> AiInputProvider::getUnitsInput(Player* player) {
	return writeBasicWith(player, player->getPossession()->getUnitsMetrics());
}

std::span<const float> AiInputProvider::getBuildingsInput(Player* player) {
	return writeBasicWith( player, player->getPossession()->getBuildingsMetrics());
}

std::span<const float> AiInputProvider::getUnitsInputWithMetric(Player* player, const db_unit_metric* prop) {
	return writeBasicWith(player, prop->getTypesVal());
}

std::span<const float> AiInputProvider::getBuildingsInputWithMetric(Player* player, const db_building_metric* prop,
                                                                    ParentBuildingType type) {
	if (type == ParentBuildingType::RESOURCE) {
		return writeBasicWith(player, prop->getValuesNormAsValForType(type));
	}
	return writeBasicWith(player, prop->getTypesVal());
}

std::span<const float> AiInputProvider::getAttackOrDefenceInput(Player* player, Player* enemy) {
	METRIC_DEFINITIONS.writeAttackOrDefence(player, enemy);
	return attackOrDefenceInput;
}

std::span<const float> AiInputProvider::getWhereAttack(Player* player, Player* enemy) {
	METRIC_DEFINITIONS.writeWhereAttack(player, enemy);
	return whereAttackInput;
}

std::span<const float> AiInputProvider::getWhereDefend(Player* player, Player* enemy) {
	METRIC_DEFINITIONS.writeWhereDefend(player, enemy);
	return whereDefendInput;
}

std::span<const float> AiInputProvider::getBuildingsTypeInput(Player* player, Player* enemy, ParentBuildingType type) {
	const auto possession = player->getPossession();
	switch (type) {
	case ParentBuildingType::OTHER: return writeBasicWith(player, possession->getBuildingsMetrics(type));
	case ParentBuildingType::DEFENCE: return writeBasicWith(player, possession->getBuildingsMetrics(type));
	case ParentBuildingType::RESOURCE: {
		METRIC_DEFINITIONS.writeResourceWithOutBonus(player, enemy);
		return buildingsResInput;
	}
	case ParentBuildingType::TECH: return writeBasicWith(player, possession->getBuildingsMetrics(type));
	case ParentBuildingType::UNITS: return writeBasicWith(player, possession->getBuildingsMetrics(type));
	default: ;
	}
}

// template <std::size_t N>
// std::span<float> AiInputProvider::writeBasic(std::array<float, N>& output, Player* player) {
// 	return METRIC_DEFINITIONS.writeBasic(output, player, Game::getPlayersMan()->getEnemyFor(player->getId()));
// }

std::span<const float> AiInputProvider::writeBasicWith(Player* player, std::span<const float> second) {
	
	auto result = METRIC_DEFINITIONS.writeBasic(player, Game::getPlayersMan()->getEnemyFor(player->getId()));//przesuniery o basic skad paczatek
	//assert(result.size() > BASIC_SIZE + second.size());
	std::ranges::copy(second, result.begin());
	return std::span(input.begin(), input.begin() + BASIC_SIZE + second.size());
}
