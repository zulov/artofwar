#include "AiInputProvider.h"

#include "Game.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiUtils.h"


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

std::span<float> AiInputProvider::getResourceInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	auto& resources = player->getResources();
	const auto basic = getBasicInput(playerId);
	copyTo(resourceIdInputSpan, basic, resources.getGatherSpeeds(), resources.getValues());

	resourceIdInputSpan[cast(ResourceInputType::FREE_WORKERS)] = player->getPossession().getFreeWorkersNumber();
	resourceIdInputSpan[cast(ResourceInputType::WORKERS)] = player->getPossession().getWorkersNumber();

	std::transform(resourceIdInputSpan.begin() + basic.size(), resourceIdInputSpan.end(), wResourceInput,
	               resourceIdInputSpan.begin() + basic.size(), std::divides<>());
	return resourceIdInputSpan;
}

std::span<float> AiInputProvider::getUnitsInput(char playerId) {
	const auto basic = getBasicInput(playerId);

	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	copyTo(unitsInputSpan, basic, player->getPossession().getUnitsMetrics());

	std::transform(unitsInputSpan.begin() + basic.size(), unitsInputSpan.end(), wUnitsSumInput,
	               unitsInputSpan.begin() + basic.size(), std::divides<>());
	return unitsInputSpan;
}

std::span<float> AiInputProvider::getBuildingsInput(char playerId) {
	const auto basic = getBasicInput(playerId);

	auto* player = Game::getPlayersMan()->getPlayer(playerId);

	copyTo(buildingsInputSpan, basic, player->getPossession().getBuildingsMetrics());

	std::transform(buildingsInputSpan.begin() + basic.size(), buildingsInputSpan.end(), wBuildingsSumInput,
	               buildingsInputSpan.begin() + basic.size(), std::divides<>());
	return buildingsInputSpan;
}

std::span<float> AiInputProvider::getUnitsInputWithMetric(char playerId, const db_unit_metric* prop) {
	copyTo(unitsWithMetricUnitSpan, getUnitsInput(playerId), prop->getParamsNorm());
	return unitsWithMetricUnitSpan;
}

std::span<float> AiInputProvider::getBuildingsInputWithMetric(char playerId, const db_building_metric* prop) {
	copyTo(basicWithMetricUnitSpan, getBuildingsInput(playerId), prop->getParamsNorm());
	return basicWithMetricUnitSpan;
}

std::span<float> AiInputProvider::getBasicInput(short id) {
	char idEnemy; //TODO do it better
	if (id == 0) {
		idEnemy = 1;
	} else {
		idEnemy = 0;
	}

	update(id, basicInputSpan.data());
	update(idEnemy, basicInputSpan.data() + magic_enum::enum_count<BasicInputType>());

	return basicInputSpan;
}

void AiInputProvider::update(short id, float* data) {
	auto player = Game::getPlayersMan()->getPlayer(id);
	data[cast(BasicInputType::RESULT)] = player->getScore();
	data[cast(BasicInputType::UNIT_NUMBER)] = player->getPossession().getUnitsNumber();
	data[cast(BasicInputType::BUILDING_NUMBER)] = player->getPossession().getBuildingsNumber();

	std::transform(data, data + magic_enum::enum_count<BasicInputType>(), wBasic, data, std::divides<>());
}