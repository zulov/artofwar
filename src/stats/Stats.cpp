#include "Stats.h"
#include <fstream>
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/ResourceActionCommand.h"
#include "commands/action/ResourceActionType.h"
#include "commands/action/UnitActionCommand.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "objects/ObjectEnums.h"
#include "objects/unit/order/UnitOrder.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/ai/AiUtils.h"
#include "simulation/env/Environment.h"
#include "utils/StringUtils.h"

#define DEFAULT_NORMALIZE_VALUE 10.f
#define INFLUENCE_DATA_SIZE 9 //TODO hard code

Stats::Stats() {
	wBasic[cast(StatsInputType::RESULT)] = 1000;
	wBasic[cast(StatsInputType::GOLD)] = 1000;
	wBasic[cast(StatsInputType::WOOD)] = 1000;
	wBasic[cast(StatsInputType::FOOD)] = 1000;
	wBasic[cast(StatsInputType::STONE)] = 1000;
	wBasic[cast(StatsInputType::ATTACK)] = 1000;
	wBasic[cast(StatsInputType::DEFENCE)] = 1000;
	wBasic[cast(StatsInputType::BASE_TO_ENEMY)] = 1000;

	wBasic[cast(StatsInputType::UNITS_ATTACK)] = 1000;
	wBasic[cast(StatsInputType::UNITS_DEFENCE)] = 1000;
	wBasic[cast(StatsInputType::UNITS_ECONOMY)] = 1000;

	wBasic[cast(StatsInputType::BUILDINGS_ATTACK)] = 100;
	wBasic[cast(StatsInputType::BUILDINGS_DEFENCE)] = 100;
	wBasic[cast(StatsInputType::BUILDINGS_ECONOMY)] = 100;

	wBasic[cast(StatsInputType::WORKERS)] = 100;

	wResourceInput[cast(ResourceInputType::GOLD_SPEED)] = 10;
	wResourceInput[cast(ResourceInputType::WOOD_SPEED)] = 10;
	wResourceInput[cast(ResourceInputType::FOOD_SPEED)] = 10;
	wResourceInput[cast(ResourceInputType::STONE_SPEED)] = 10;

	wResourceInput[cast(ResourceInputType::GOLD_VALUE)] = 1000;
	wResourceInput[cast(ResourceInputType::WOOD_VALUE)] = 1000;
	wResourceInput[cast(ResourceInputType::FOOD_VALUE)] = 1000;
	wResourceInput[cast(ResourceInputType::STONE_VALUE)] = 1000;

	wResourceInput[cast(ResourceInputType::PLAYER_SCORE)] = 1000;
}

Stats::~Stats() {
	saveAll(1, 1);
	clear();
	delete []basicInput;
	delete []resourceIdInput;
	delete []basicInputWithParams;
}

void Stats::init() {
	clear();
	basicInput = new float[magic_enum::enum_count<StatsInputType>() * 2];
	basicInputSpan = std::span{basicInput, magic_enum::enum_count<StatsInputType>() * 2};

	resourceIdInput = new float[magic_enum::enum_count<ResourceInputType>()];
	resourceIdInputSpan = std::span(resourceIdInput, magic_enum::enum_count<ResourceInputType>());

	basicInputWithParams = new float[magic_enum::enum_count<StatsInputType>() * 2 + AI_PROPS_SIZE];
	basicInputWithParamsSpan = std::span{
		basicInputWithParams, magic_enum::enum_count<StatsInputType>() * 2 + AI_PROPS_SIZE
	};

	for (auto allPlayer : Game::getPlayersMan()->getAllPlayers()) {
		statsPerPlayer.push_back(new float[magic_enum::enum_count<StatsInputType>()]);
	}
}

std::string Stats::getInputData(char player) {
	return join(getBasicInput(player));
}

void Stats::add(GeneralActionCommand* command) {
	const auto player = command->player;

	const std::string input = getInputData(player);

	joinAndPush(mainOrder, player, input, getOutput(command));

	auto opt = Game::getPlayersMan()
	           ->getPlayer(command->player)->getNextLevelForBuilding(command->id);
	if (opt.has_value()) {
		auto& createOutput = opt.value()->aiPropsLevelUp->getParamsNormAsString();
		joinAndPush(buildLevelUpId, player, input, createOutput);
	}
}

void Stats::add(ResourceActionCommand* command) {
	const auto player = command->player;

	const std::string input = getInputData(player);

	joinAndPush(mainOrder, player, input, getOutput(command),command->resources.size());
}

void Stats::add(BuildingActionCommand* command) {
	const std::string input = getInputData(command->player);
	const std::string basicOutput = getOutput(command);
	auto player = Game::getPlayersMan()->getPlayer(command->player);

	joinAndPush(mainOrder, command->player, input, basicOutput, command->buildings.size());
	for (auto building : command->buildings) {
		if (command->action == BuildingActionType::UNIT_CREATE) {
			auto& createOutput = player->getLevelForUnit(command->id)->aiProps->getParamsNormAsString();
			joinAndPush(unitCreateId, command->player, input, createOutput);

			const std::string inputWithAiProps = input + ";" + createOutput;
			joinAndPush(unitCreatePos, command->player, inputWithAiProps, getCreateUnitPosOutput(building));
		} else if (command->action == BuildingActionType::UNIT_LEVEL) {
			auto opt = player->getNextLevelForUnit(command->id);
			if (opt.has_value()) {
				auto& createOutput = opt.value()->aiPropsLevelUp->getParamsNormAsString();
				joinAndPush(unitUpgradeId, command->player, input, createOutput);

				const std::string inputWithAiProps = input + ";" + createOutput;
				joinAndPush(unitLevelUpPos, command->player, inputWithAiProps, getLevelUpUnitPosOutput(building));
			}
		}
	}
}

void Stats::add(CreationCommand* command) {
	if (command->objectType != ObjectType::BUILDING) {
		Game::getLog()->Write(0, "ERROR - WRONG command");
	}

	const auto player = command->player;

	const std::string input = getInputData(player);

	joinAndPush(mainOrder, player, input, getOutput(command));
	auto& createOutput = Game::getPlayersMan()->getPlayer(command->player)
	                                          ->getLevelForBuilding(command->id)->aiProps->getParamsNormAsString();
	joinAndPush(buildingCreateId, player, input, createOutput);
	const std::string inputWithAiProps = input + ";" + createOutput;
	joinAndPush(buildingCreatePos, player, inputWithAiProps, getCreateBuildingPosOutput(command));
}

void Stats::joinAndPush(std::vector<std::string>* array, char player, std::string input, const std::string& output,
                        int number) {
	if (!output.empty()) {
		input.append(";;").append(output);
		for (int i = 0; i < number; ++i) {
			array[player].push_back(input);
		}
	}
}

void Stats::save(int i, std::vector<std::string>* array, std::string fileName) const {
	std::ofstream outFile;

	outFile.open(fileName, std::ios_base::app);
	for (const auto& e : array[i]) {
		outFile << e << "\n";
	}
	outFile.close();
}

void Stats::saveBatch(int i, std::vector<std::string>* array, std::string name, int size) const {
	if (array[i].size() >= size) {
		save(i, array, "result/ai/" + name + "_" + std::to_string(i) + ".csv");
		save(i, array, "result/ai/" + name + "_combine.csv");

		array[i].clear();
	}
}

void Stats::saveAll(int big, int small) {
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		saveBatch(i, mainOrder, "main", big);

		saveBatch(i, buildingCreateId, "buildId", small);
		saveBatch(i, buildingCreatePos, "buildPos", small);

		saveBatch(i, unitCreateId, "unitId", small);
		saveBatch(i, unitCreatePos, "unitPos", small);

		saveBatch(i, unitOrderId, "unitOrderId", small);

		saveBatch(i, buildLevelUpId, "buildLevelUpId", small);

		saveBatch(i, unitUpgradeId, "unitUpgradeId", small);
		saveBatch(i, unitLevelUpPos, "unitLevelUpPos", small);

		saveBatch(i, resourceId, "resourceId", small);
	}
}

void Stats::save() {
	saveAll(SAVE_BATCH_SIZE, SAVE_BATCH_SIZE_MINI);
}

void Stats::update(short id) {
	float* data = statsPerPlayer.at(id);
	auto player = Game::getPlayersMan()->getPlayer(id);
	data[cast(StatsInputType::RESULT)] = player->getScore();
	data[cast(StatsInputType::GOLD)] = player->getResources().getValues()[0];
	data[cast(StatsInputType::WOOD)] = player->getResources().getValues()[1];
	data[cast(StatsInputType::FOOD)] = player->getResources().getValues()[2];
	data[cast(StatsInputType::STONE)] = player->getResources().getValues()[3];
	data[cast(StatsInputType::ATTACK)] = player->getAttackScore();
	data[cast(StatsInputType::DEFENCE)] = player->getDefenceScore();
	data[cast(StatsInputType::BASE_TO_ENEMY)] = Game::getEnvironment()->getDistToEnemy(player); //TODO ale do którego

	data[cast(StatsInputType::UNITS_ATTACK)] = player->getUnitsVal(ValueType::ATTACK);
	data[cast(StatsInputType::UNITS_DEFENCE)] = player->getUnitsVal(ValueType::DEFENCE);
	data[cast(StatsInputType::UNITS_ECONOMY)] = player->getUnitsVal(ValueType::ECON);

	data[cast(StatsInputType::BUILDINGS_ATTACK)] = player->getBuildingsVal(ValueType::ATTACK);
	data[cast(StatsInputType::BUILDINGS_DEFENCE)] = player->getBuildingsVal(ValueType::DEFENCE);
	data[cast(StatsInputType::BUILDINGS_ECONOMY)] = player->getBuildingsVal(ValueType::ECON);

	data[cast(StatsInputType::WORKERS)] = player->getWorkersNumber();

	std::transform(data, data + magic_enum::enum_count<StatsInputType>(), wBasic, data, std::divides<>());
}


int Stats::getScoreFor(short id) const {
	return Game::getPlayersMan()->getPlayer(id)->getScore();
}

std::span<float> Stats::getBasicInput(short id) {
	char idEnemy; //TODO do it better
	if (id == 0) {
		idEnemy = 1;
	} else {
		idEnemy = 0;
	}
	update(id); //TODO PERFORMANCE robic rzadzej
	update(idEnemy);
	auto stats = statsPerPlayer.at(id);
	auto enemy = statsPerPlayer.at(idEnemy); //TODO BUG wybrac wroga
	std::copy(stats, stats + magic_enum::enum_count<StatsInputType>(), basicInputSpan.begin());
	std::copy(enemy, enemy + magic_enum::enum_count<StatsInputType>(),
	          basicInputSpan.begin() + magic_enum::enum_count<StatsInputType>());
	return basicInputSpan;
}

void Stats::clear() {
	clear_vector(statsPerPlayer);
}

std::string Stats::getCreateBuildingPosOutput(CreationCommand* command) const {
	auto& data = Game::getEnvironment()->getInfluenceDataAt(command->player, command->position);

	return join(data.begin(), data.end());
}

std::string Stats::getCreateUnitPosOutput(Building* building) const {
	float output[INFLUENCE_DATA_SIZE + 1];

	std::fill_n(output, INFLUENCE_DATA_SIZE + 1, 0.f);
	auto buildingPos = building->getPosition();

	auto& data = Game::getEnvironment()->getInfluenceDataAt(building->getPlayer(), {buildingPos.x_, buildingPos.z_});
	std::copy(data.begin(), data.end(), output);

	output[INFLUENCE_DATA_SIZE] = building->getQueue()->getSize() / DEFAULT_NORMALIZE_VALUE;
	return join(output, output + INFLUENCE_DATA_SIZE + 1);
}

std::string Stats::getLevelUpUnitPosOutput(Building* building) const {
	auto queueSize = building->getQueue()->getSize() / DEFAULT_NORMALIZE_VALUE;
	std::ostringstream ss;
	ss << queueSize;
	return std::string(ss.str());
}

std::string Stats::getOutput(CreationCommand* command) const {
	return getOutput(StatsOutputType::CREATE_BUILDING);
}

std::string Stats::getOutput(UpgradeCommand* command) const {
	switch (command->type) {
	case QueueActionType::UNIT_LEVEL:
		return getOutput({StatsOutputType::LEVEL_UP_UNIT});
	case QueueActionType::BUILDING_LEVEL:
		return getOutput({StatsOutputType::LEVEL_UP_BUILDING});
	default: ;
	}
}

std::string Stats::getOutput(ResourceActionCommand* command) const {
	//TODO
	switch (command->action) {
	case ResourceActionType::COLLECT:
		return getOutput(StatsOutputType::IDLE);
	case ResourceActionType::CANCEL:
		return getOutput(StatsOutputType::IDLE);
	default: ;
	}
}

std::string Stats::getOutput(BuildingActionCommand* command) const {
	switch (command->action) {
	case BuildingActionType::UNIT_CREATE:
		return getOutput(StatsOutputType::CREATE_UNIT);
	case BuildingActionType::UNIT_LEVEL:
		return getOutput(StatsOutputType::LEVEL_UP_UNIT);
	case BuildingActionType::UNIT_UPGRADE:
		//TODO dodac kiedyś
		break;
	default: ;
	}
}

std::string Stats::getOutput(GeneralActionCommand* command) const {
	return getOutput(StatsOutputType::LEVEL_UP_BUILDING);
}

std::string Stats::getOutput(StatsOutputType stat) const {
	float output[magic_enum::enum_count<StatsOutputType>()];
	std::fill_n(output, magic_enum::enum_count<StatsOutputType>(), 0.f);

	output[cast(stat)] = 1;

	return join(output, output + magic_enum::enum_count<StatsOutputType>());
}

std::string Stats::getResourceIdOutput(UnitActionCommand* command) const {
	float output[4];
	std::fill_n(output, 4, 0.f);
	output[command->order->getToUseId()] = 1;
	return join(output, output + 4);
}

std::string Stats::getResourceIdInputAsString(char playerId) {
	return join(getResourceIdInput(playerId));
}

std::span<float> Stats::getResourceIdInput(char playerId) {
	auto player = Game::getPlayersMan()->getPlayer(playerId);
	auto& resources = player->getResources();

	copyTo(resourceIdInputSpan, resources.getGatherSpeeds(), resources.getValues());

	resourceIdInputSpan[magic_enum::enum_count<ResourceInputType>() - 1] = player->getScore();

	std::transform(resourceIdInputSpan.begin(), resourceIdInputSpan.end(), wResourceInput, resourceIdInputSpan.begin(),
	               std::divides<>());
	return resourceIdInputSpan;
}

std::span<float> Stats::getBasicInputWithParams(char playerId, const db_ai_property* prop) {
	auto basicInput = getBasicInput(playerId);

	copyTo(basicInputWithParamsSpan, basicInput, prop->getParamsNormAsSpan());

	return basicInputWithParamsSpan;
}

void Stats::add(UnitActionCommand* command) {
	const auto playerId = command->player;

	const std::string input = getInputData(playerId);

	joinAndPush(unitOrderId, playerId, input, getOutput(command));
	if (command->order->getId() == static_cast<char>(UnitAction::COLLECT)) {
		joinAndPush(resourceId, playerId, getResourceIdInputAsString(playerId), getResourceIdOutput(command),
		            command->order->getSize());
	}
}

std::string Stats::getOutput(UnitActionCommand* command) const {
	float output[magic_enum::enum_count<StatsOrderOutputType>()];
	std::fill_n(output, magic_enum::enum_count<StatsOrderOutputType>(), 0.f);

	switch (static_cast<UnitAction>(command->order->getId())) {
	case UnitAction::GO:
		output[cast(StatsOrderOutputType::ORDER_GO)] = 1;
		break;
	case UnitAction::STOP:
		output[cast(StatsOrderOutputType::ORDER_STOP)] = 1;
		break;
	case UnitAction::CHARGE:
		output[cast(StatsOrderOutputType::ORDER_CHARGE)] = 1;
		break;
	case UnitAction::ATTACK:
		output[cast(StatsOrderOutputType::ORDER_ATTACK)] = 1;
		break;
	case UnitAction::DEAD:
		output[cast(StatsOrderOutputType::ORDER_DEAD)] = 1;
		break;
	case UnitAction::DEFEND:
		output[cast(StatsOrderOutputType::ORDER_DEFEND)] = 1;
		break;
	case UnitAction::FOLLOW:
		output[cast(StatsOrderOutputType::ORDER_FOLLOW)] = 1;
		break;
	case UnitAction::COLLECT:
		output[cast(StatsOrderOutputType::ORDER_COLLECT)] = 1;
		break;
	default: ;
	}

	return join(output, output + magic_enum::enum_count<StatsOrderOutputType>());
}
