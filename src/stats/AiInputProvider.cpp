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

	wResourceInput[cast(ResourceInputType::WORKERS)] = 100.f;

	basicInputSpan = std::span(basicInput);
	resourceIdInputSpan = std::span(resourceIdInput);
	basicWithParamsSpan = std::span(basicWithParams);
}

std::span<float> AiInputProvider::getResourceInput(char playerId) {
	auto* player = Game::getPlayersMan()->getPlayer(playerId);
	auto& resources = player->getResources();
	const auto basic = getBasicInput(playerId);
	copyTo(resourceIdInputSpan, basic, resources.getGatherSpeeds(), resources.getValues());

	resourceIdInputSpan.back() = player->getPossession().getWorkersNumber();

	std::transform(resourceIdInputSpan.begin() + basic.size(), resourceIdInputSpan.end(), wResourceInput,
	               resourceIdInputSpan.begin() + basic.size(), std::divides<>());
	return resourceIdInputSpan;
}

std::span<float> AiInputProvider::getBasicInputWithMetric(char playerId, const db_basic_metric* prop) {
	auto basicInput = getBasicInput(playerId);

	copyTo(basicWithParamsSpan, basicInput, prop->getParamsNormAsSpan());

	return basicWithParamsSpan;
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
