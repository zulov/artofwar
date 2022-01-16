#include "AiInputProvider.h"

#include "Game.h"
#include "math/SpanUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiMetric.h"
#include "player/ai/AiUtils.h"


std::span<float> AiInputProvider::getResourceInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	auto newSpan = getBasicInput(resourceIdInputSpan, player);
	auto& data = METRIC_DEFINITIONS.getResourceNorm(player->getResources(), player->getPossession());
	std::ranges::copy(data, newSpan.begin());

	assert(validateSpan(__LINE__, __FILE__, resourceIdInputSpan));
	return resourceIdInputSpan;
}

std::span<float> AiInputProvider::getUnitsInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	auto newSpan = getBasicInput(unitsInputSpan, player);

	std::ranges::copy(player->getPossession().getUnitsMetrics(), newSpan.begin());

	assert(validateSpan(__LINE__, __FILE__, unitsInputSpan));
	return unitsInputSpan;
}

std::span<float> AiInputProvider::getBuildingsInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	auto newSpan = getBasicInput(buildingsInputSpan, player);

	std::ranges::copy(player->getPossession().getBuildingsMetrics(), newSpan.begin());

	assert(validateSpan(__LINE__, __FILE__, buildingsInputSpan));
	return buildingsInputSpan;
}

std::span<float> AiInputProvider::getUnitsInputWithMetric(char playerId, const db_unit_metric* prop) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	auto newSpan = getBasicInput(unitsWithMetricUnitSpan, player);

	std::ranges::copy(prop->getValuesNormSmall(), newSpan.begin());

	assert(validateSpan(__LINE__, __FILE__, unitsWithMetricUnitSpan));
	return unitsWithMetricUnitSpan;
}

std::span<float> AiInputProvider::getBuildingsInputWithMetric(char playerId, const db_building_metric* prop) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	auto newSpan = getBasicInput(basicWithMetricUnitSpan, player);

	std::ranges::copy(prop->getValuesNormSmall(), newSpan.begin());

	assert(validateSpan(__LINE__, __FILE__, basicWithMetricUnitSpan));
	return basicWithMetricUnitSpan;
}

std::span<float> AiInputProvider::getBasicInput(std::span<float> dest, Player* player) {
	char idEnemy = Game::getPlayersMan()->getEnemyFor(player->getId());
	auto& data = METRIC_DEFINITIONS.getBasicNorm(player, Game::getPlayersMan()->getPlayer(idEnemy));
	assert(validateSpan(__LINE__, __FILE__, data));
	std::copy(data.begin(), data.end(), dest.begin());

	return std::span(dest.end(), dest.size() - data.size());
}
