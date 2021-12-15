#include "AiInputProvider.h"

#include "Game.h"
#include "math/SpanUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiUtils.h"
#include "utils/OtherUtils.h"


AiInputProvider::AiInputProvider() {
	wBasic[cast(BasicInputType::RESULT)] = 1000.f;
	wBasic[cast(BasicInputType::UNIT_NUMBER)] = 100.f;
	wBasic[cast(BasicInputType::BUILDING_NUMBER)] = 100.f;

	wResourceInput[cast(ResourceInputType::GOLD_SPEED)] = 10.f;
	wResourceInput[cast(ResourceInputType::WOOD_SPEED)] = 10.f;
	wResourceInput[cast(ResourceInputType::FOOD_SPEED)] = 10.f;
	wResourceInput[cast(ResourceInputType::STONE_SPEED)] = 10.f;

	wResourceInput[cast(ResourceInputType::GOLD_VALUE)] = 1000.f;
	wResourceInput[cast(ResourceInputType::WOOD_VALUE)] = 1000.f;
	wResourceInput[cast(ResourceInputType::FOOD_VALUE)] = 1000.f;
	wResourceInput[cast(ResourceInputType::STONE_VALUE)] = 1000.f;

	wResourceInput[cast(ResourceInputType::FREE_WORKERS)] = 100.f;
	wResourceInput[cast(ResourceInputType::WORKERS)] = 100.f;

	wUnitsSumInput[cast(UnitMetric::DEFENCE)] = 50000.f; //TODO to nie sa wartosci dla jednostki tylko dla sumy
	wUnitsSumInput[cast(UnitMetric::DISTANCE_ATTACK)] = 10000.f;
	wUnitsSumInput[cast(UnitMetric::CLOSE_ATTACK)] = 10000.f;
	wUnitsSumInput[cast(UnitMetric::CHARGE_ATTACK)] = 1000.f;
	wUnitsSumInput[cast(UnitMetric::BUILDING_ATTACK)] = 10000.f;

	wBuildingsSumInput[cast(BuildingMetric::DEFENCE)] = 200000.f; //TODO to nie sa wartosci dla jednostki tylko dla sumy
	wBuildingsSumInput[cast(BuildingMetric::DISTANCE_ATTACK)] = 10000.f;
	wBuildingsSumInput[cast(BuildingMetric::PROD_DEFENCE)] = 20000.f;
	wBuildingsSumInput[cast(BuildingMetric::PROD_DISTANCE_ATTACK)] = 20000.f;
	wBuildingsSumInput[cast(BuildingMetric::PROD_CLOSE_ATTACK)] = 20000.f;
	wBuildingsSumInput[cast(BuildingMetric::PROD_CHARGE_ATTACK)] = 20000.f;
	wBuildingsSumInput[cast(BuildingMetric::PROD_BUILDING_ATTACK)] = 20000.f;
}

void AiInputProvider::applyWeights(std::span<float> dest, float* weights, size_t skip) {
	std::transform(dest.begin() + skip, dest.end(), weights, dest.begin() + skip, std::divides<>());
	assert(validateSpan(__LINE__, __FILE__, dest));
}

std::span<float> AiInputProvider::getResourceInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	auto& resources = player->getResources();
	auto& possession = player->getPossession();

	const auto basic = getBasicInput(player);
	copyTo(resourceIdInputSpan, basic, resources.getGatherSpeeds(), resources.getValues());

	resourceIdInputSpan[cast(ResourceInputType::FREE_WORKERS) + basic.size()] = possession.getFreeWorkersNumber();
	resourceIdInputSpan[cast(ResourceInputType::WORKERS) + basic.size()] = possession.getWorkersNumber();

	applyWeights(resourceIdInputSpan, wResourceInput, basic.size());

	return resourceIdInputSpan;
}

std::span<float> AiInputProvider::getUnitsInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	const auto basic = getBasicInput(player);

	copyTo(unitsInputSpan, basic, player->getPossession().getUnitsMetrics());

	applyWeights(unitsInputSpan, wUnitsSumInput, basic.size());

	return unitsInputSpan;
}

std::span<float> AiInputProvider::getBuildingsInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	const auto basic = getBasicInput(player);

	copyTo(buildingsInputSpan, basic, player->getPossession().getBuildingsMetrics());

	applyWeights(buildingsInputSpan, wBuildingsSumInput, basic.size());

	return buildingsInputSpan;
}

std::span<float> AiInputProvider::getUnitsInputWithMetric(char playerId, const db_unit_metric* prop) {
	return copyTo(unitsWithMetricUnitSpan, getUnitsInput(playerId), prop->getParamsNorm());
}

std::span<float> AiInputProvider::getBuildingsInputWithMetric(char playerId, const db_building_metric* prop) {
	return copyTo(basicWithMetricUnitSpan, getBuildingsInput(playerId), prop->getParamsNorm());
}

std::span<float> AiInputProvider::getBasicInput(short id) {
	return getBasicInput(Game::getPlayersMan()->getPlayer(id));
}

std::span<float> AiInputProvider::getBasicInput(Player* player) {
	char idEnemy = Game::getPlayersMan()->getEnemyFor(player->getId());

	updateBasic(player, basicInputSpan.data());
	updateBasic(Game::getPlayersMan()->getPlayer(idEnemy),
	            basicInputSpan.data() + magic_enum::enum_count<BasicInputType>());
	assert(validateSpan(__LINE__, __FILE__, basicInputSpan));
	return basicInputSpan;
}

void AiInputProvider::updateBasic(Player* player, float* data) {
	data[cast(BasicInputType::RESULT)] = player->getScore();
	data[cast(BasicInputType::UNIT_NUMBER)] = player->getPossession().getUnitsNumber();
	data[cast(BasicInputType::BUILDING_NUMBER)] = player->getPossession().getBuildingsNumber();

	std::transform(data, data + magic_enum::enum_count<BasicInputType>(), wBasic, data, std::divides<>());
}
