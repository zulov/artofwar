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
#include "simulation/env/Environment.h"
#include "utils/StringUtils.h"

#define DEFAULT_NORMALIZE_VALUE 10.f
#define INFLUENCE_DATA_SIZE 9 //TODO hard code

Stats::Stats() {
	weights[cast(StatsInputType::RESULT)] = 1000;
	weights[cast(StatsInputType::GOLD)] = 1000;
	weights[cast(StatsInputType::WOOD)] = 1000;
	weights[cast(StatsInputType::FOOD)] = 1000;
	weights[cast(StatsInputType::STONE)] = 1000;
	weights[cast(StatsInputType::ATTACK)] = 1000;
	weights[cast(StatsInputType::DEFENCE)] = 1000;
	weights[cast(StatsInputType::BASE_TO_ENEMY)] = 1000;
	weights[cast(StatsInputType::UNITS)] = 1000;
	weights[cast(StatsInputType::BUILDINGS)] = 100;
	weights[cast(StatsInputType::WORKERS)] = 100;
}

Stats::~Stats() {
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		saveBatch(i, mainOrder, "main", 1);
		saveBatch(i, buildingCreateId, "buildId", 1);
		saveBatch(i, buildingCreatePos, "buildPos", 1);
		saveBatch(i, unitCreateId, "unitId", 1);
		saveBatch(i, unitCreatePos, "unitPos", 1);
		saveBatch(i, unitOrderId, "unitOrderId", 1);
		saveBatch(i, buildLevelUpId, "unitOrderId", 1);
		saveBatch(i, unitUpgradeId, "unitUpgradeId", 1);
		saveBatch(i, unitLevelUpPos, "unitLevelUpPos", 1);
	}
	clear();
	delete []input;
}

void Stats::init() {
	clear();
	input = new float[magic_enum::enum_count<StatsInputType>() * 2];
	for (auto allPlayer : Game::getPlayersMan()->getAllPlayers()) {
		statsPerPlayer.push_back(new float[magic_enum::enum_count<StatsInputType>()]);
	}
}

std::string Stats::getInputData(char player) {
	const auto input = getInputFor(player);
	return join(input, input + magic_enum::enum_count<StatsInputType>() * 2);
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

	joinAndPush(mainOrder, player, input, getOutput(command));
}

void Stats::add(BuildingActionCommand* command) {
	const std::string input = getInputData(command->player);
	const std::string basicOutput = getOutput(command);
	auto player = Game::getPlayersMan()->getPlayer(command->player);


	for (auto building : command->buildings) {
		joinAndPush(mainOrder, command->player, input, basicOutput);
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

void Stats::add(UnitActionCommand* command) {
	const auto player = command->player;

	const std::string input = getInputData(player);

	joinAndPush(unitOrderId, player, input, getOutput(command));
}

void Stats::add(CreationCommand* command) {
	if (command->objectType != ObjectType::BUILDING) {
		Game::getLog()->Write(0, "ERROR - WRONG command");
	}

	const auto player = command->player;

	const std::string input = getInputData(player);

	joinAndPush(mainOrder, player, input, getOutput(command));
	auto& createOutput = Game::getPlayersMan()
	                     ->getPlayer(command->player)->getLevelForBuilding(command->id)->aiProps->
	                     getParamsNormAsString();
	joinAndPush(buildingCreateId, player, input, createOutput);
	const std::string inputWithAiProps = input + ";" + createOutput;
	joinAndPush(buildingCreatePos, player, inputWithAiProps, getCreateBuildingPosOutput(command));
}

void Stats::joinAndPush(std::vector<std::string>* array, char player, std::string input, const std::string& output) {
	if (!output.empty()) {
		input.append(";;").append(output);
		array[player].push_back(input);
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

void Stats::save() {
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		saveBatch(i, mainOrder, "main", SAVE_BATCH_SIZE);

		saveBatch(i, buildingCreateId, "buildId", SAVE_BATCH_SIZE_MINI);
		saveBatch(i, buildingCreatePos, "buildPos", SAVE_BATCH_SIZE_MINI);

		saveBatch(i, unitCreateId, "unitId", SAVE_BATCH_SIZE_MINI);
		saveBatch(i, unitCreatePos, "unitPos", SAVE_BATCH_SIZE_MINI);

		saveBatch(i, unitOrderId, "unitOrderId", SAVE_BATCH_SIZE_MINI);

		saveBatch(i, buildLevelUpId, "buildLevelUpId", SAVE_BATCH_SIZE_MINI);

		saveBatch(i, unitUpgradeId, "unitUpgradeId", SAVE_BATCH_SIZE_MINI);
		saveBatch(i, unitLevelUpPos, "unitLevelUpPos", SAVE_BATCH_SIZE_MINI);
	}
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
	data[cast(StatsInputType::UNITS)] = player->getUnitsNumber();
	data[cast(StatsInputType::BUILDINGS)] = player->getBuildingsNumber();
	data[cast(StatsInputType::WORKERS)] = player->getWorkersNumber();
	std::transform(data, data + magic_enum::enum_count<StatsInputType>(), weights, data, std::divides<>());
}


int Stats::getScoreFor(short id) const {
	return Game::getPlayersMan()->getPlayer(id)->getScore();
}

float* Stats::getInputFor(short id) {
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
	std::copy(stats, stats + magic_enum::enum_count<StatsInputType>(), input);
	std::copy(enemy, enemy + magic_enum::enum_count<StatsInputType>(),
	          input + magic_enum::enum_count<StatsInputType>());
	return input;
}

void Stats::clear() {
	clear_vector(statsPerPlayer);
}

std::string Stats::getCreateBuildingPosOutput(CreationCommand* command) const {
	float output[INFLUENCE_DATA_SIZE];

	std::fill_n(output, INFLUENCE_DATA_SIZE, 0.f);

	Game::getEnvironment()->writeInInfluenceDataAt(output, command->player, command->position);

	return join(output, output + INFLUENCE_DATA_SIZE);
}

std::string Stats::getCreateUnitPosOutput(Building* building) const {
	float output[INFLUENCE_DATA_SIZE + 1];

	std::fill_n(output, INFLUENCE_DATA_SIZE + 1, 0.f);
	auto buildingPos = building->getPosition();

	Game::getEnvironment()->writeInInfluenceDataAt(output, building->getPlayer(), {buildingPos.x_, buildingPos.z_});

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
	float output[magic_enum::enum_count<StatsOutputType>()];
	std::fill_n(output, magic_enum::enum_count<StatsOutputType>(), 0.f);

	output[cast(StatsOutputType::CREATE_BUILDING)] = 1;

	return join(output, output + magic_enum::enum_count<StatsOutputType>());
}

std::string Stats::getOutput(UpgradeCommand* command) const {
	float output[magic_enum::enum_count<StatsOutputType>()];
	std::fill_n(output, magic_enum::enum_count<StatsOutputType>(), 0.f);

	switch (command->type) {

	case QueueActionType::UNIT_LEVEL:
		output[cast(StatsOutputType::LEVEL_UP_UNIT)] = 1;
		break;
	case QueueActionType::BUILDING_LEVEL:
		output[cast(StatsOutputType::LEVEL_UP_BUILDING)] = 1;
		break;
	default: ;
	}

	return join(output, output + magic_enum::enum_count<StatsOutputType>());
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

std::string Stats::getOutput(ResourceActionCommand* command) const {
	//TODO
	float output[magic_enum::enum_count<StatsOutputType>()];
	std::fill_n(output, magic_enum::enum_count<StatsOutputType>(), 0.f);
	switch (command->action) {
	case ResourceActionType::COLLECT: break;
	case ResourceActionType::CANCEL: break;
	default: ;
	}
	return join(output, output + magic_enum::enum_count<StatsOutputType>());
}

std::string Stats::getOutput(BuildingActionCommand* command) const {
	float output[magic_enum::enum_count<StatsOutputType>()];
	std::fill_n(output, magic_enum::enum_count<StatsOutputType>(), 0.f);
	switch (command->action) {
	case BuildingActionType::UNIT_CREATE:
		output[cast(StatsOutputType::CREATE_UNIT)] = 1;
		break;
	case BuildingActionType::UNIT_LEVEL:
		output[cast(StatsOutputType::LEVEL_UP_UNIT)] = 1;
		break;
	case BuildingActionType::UNIT_UPGRADE:
		//TODO dodac kieyś
		break;
	default: ;
	}
	return join(output, output + magic_enum::enum_count<StatsOutputType>());
}

std::string Stats::getOutput(GeneralActionCommand* command) const {
	float output[magic_enum::enum_count<StatsOutputType>()];
	std::fill_n(output, magic_enum::enum_count<StatsOutputType>(), 0.f);

	output[cast(StatsOutputType::LEVEL_UP_BUILDING)] = 1;

	return join(output, output + magic_enum::enum_count<StatsOutputType>());
}
