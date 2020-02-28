#include "Stats.h"
#include "Game.h"
#include "StatsEnums.h"
#include "StringUtils.h"
#include "objects/ObjectEnums.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "commands/action/UnitActionCommand.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/ResourceActionCommand.h"
#include "commands/action/ResourceActionType.h"
#include "commands/action/BuildingActionType.h"
#include "objects/order/enums/UnitAction.h"
#include <fstream>


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
	clear();
	delete []input;
}

void Stats::init() {
	clear();
	input = new float[INPUT_STATS_SIZE];
	for (auto allPlayer : Game::getPlayersMan()->getAllPlayers()) {
		statsPerPlayer.push_back(new float[STATS_PER_PLAYER_SIZE]);
	}
}

std::string Stats::getInputData(char player) {
	const auto input = getInputFor(player);
	return join(input, input + INPUT_STATS_SIZE);
}

void Stats::add(UpgradeCommand* command) {
	const auto player = command->player;

	const std::string data = getInputData(player);

	appendOutput(player, data, getOutput(command));
}

void Stats::add(ResourceActionCommand* command) {
	const auto player = command->player;

	const std::string data = getInputData(player);

	appendOutput(player, data, getOutput(command));
}

void Stats::add(BuildingActionCommand* command) {
	const auto player = command->player;

	const std::string data = getInputData(player);

	appendOutput(player, data, getOutput(command));
}

void Stats::add(UnitActionCommand* command) {
	const auto player = command->player;

	const std::string data = getInputData(player);

	appendOutput(player, data, getOutput(command));
}

void Stats::add(CreationCommand* command) {
	const auto player = command->player;

	const std::string data = getInputData(player);

	appendOutput(player, data, getOutput(command));
}

void Stats::appendOutput(char player, std::string data, std::string& output) {
	data.append(";;").append(output);
	dataToSavePerPlayer[player].push_back(data);
}

void Stats::save() {
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		if (dataToSavePerPlayer[i].size() >= SAVE_BATCH_SIZE) {
			std::ofstream outFile;
			std::string name = "Data/ai/test" + std::to_string(i) + ".csv";

			outFile.open(name, std::ios_base::app);
			for (const auto& e : dataToSavePerPlayer[i]) {
				outFile << e << "\n";
			}
			outFile.close();
			dataToSavePerPlayer[i].clear();
		}
	}
}

void Stats::addBuildLevel(short id, char playerId) {
	//TODO zrobic cos
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
	std::transform(data, data + STATS_PER_PLAYER_SIZE, weights, data, std::divides<>());
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
	update(id); //TODO ERFORMANCE robic rzadzej
	update(idEnemy);
	auto stats = statsPerPlayer.at(id);
	auto enemy = statsPerPlayer.at(idEnemy); //TODO BUG wybrac wroga
	std::copy(stats, stats + STATS_PER_PLAYER_SIZE, input);
	std::copy(enemy, enemy + STATS_PER_PLAYER_SIZE, input + STATS_PER_PLAYER_SIZE);
	return input;
}

void Stats::clear() {
	clear_vector(statsPerPlayer);
}

std::string Stats::getOutput(CreationCommand* command) const {
	float output[STATS_OUTPUT_SIZE];
	std::fill_n(output,STATS_OUTPUT_SIZE, 0);
	//TODO command->type;, command->id; wybrac ktore wzmocnic
	switch (command->objectType) {
	case ObjectType::UNIT:
	{
		output[cast(StatsOutputType::CREATE_UNIT_ATTACK)] = 1;
		output[cast(StatsOutputType::CREATE_UNIT_DEFENCE)] = 1;
		output[cast(StatsOutputType::CREATE_UNIT_ECON)] = 1;
	}
	break;
	case ObjectType::BUILDING:
		output[cast(StatsOutputType::CREATE_BUILDING_ATTACK)] = 1;
		output[cast(StatsOutputType::CREATE_BUILDING_DEFENCE)] = 1;
		output[cast(StatsOutputType::CREATE_BUILDING_ECON)] = 1;
		break;
	}

	return join(output, output + STATS_OUTPUT_SIZE);
}

std::string Stats::getOutput(UpgradeCommand* command) const {
	float output[STATS_OUTPUT_SIZE];
	std::fill_n(output,STATS_OUTPUT_SIZE, 0);
	//TODO command->type;, command->id; wybrac ktore wzmocnic

	output[cast(StatsOutputType::UPGRADE_ATTACK)] = 1;
	output[cast(StatsOutputType::UPGRADE_DEFENCE)] = 1;
	output[cast(StatsOutputType::UPGRADE_ECON)] = 1;
	return join(output, output + STATS_OUTPUT_SIZE);
}

std::string Stats::getOutput(UnitActionCommand* command) const {
	float output[STATS_OUTPUT_SIZE];
	std::fill_n(output,STATS_OUTPUT_SIZE, 0);
	//TODO command->type;, command->id; wybrac ktore wzmocnic
	switch (static_cast<UnitAction>(command->futureAim->getId())) {
	case UnitAction::GO:
		output[cast(StatsOutputType::ORDER_GO)] = 1;
		break;
	case UnitAction::STOP:
		output[cast(StatsOutputType::ORDER_STOP)] = 1;
		break;
	case UnitAction::CHARGE:
		output[cast(StatsOutputType::ORDER_CHARGE)] = 1;
		break;
	case UnitAction::ATTACK:
		output[cast(StatsOutputType::ORDER_ATTACK)] = 1;
		break;
	case UnitAction::DEAD:
		output[cast(StatsOutputType::ORDER_DEAD)] = 1;
		break;
	case UnitAction::DEFEND:
		output[cast(StatsOutputType::ORDER_DEFEND)] = 1;
		break;
	case UnitAction::FOLLOW:
		output[cast(StatsOutputType::ORDER_FOLLOW)] = 1;
		break;
	case UnitAction::COLLECT:
		output[cast(StatsOutputType::ORDER_COLLECT)] = 1;
		break;
	default: ;
	}

	return join(output, output + STATS_OUTPUT_SIZE);
}

std::string Stats::getOutput(ResourceActionCommand* command) const {
	float output[STATS_OUTPUT_SIZE];
	std::fill_n(output,STATS_OUTPUT_SIZE, 0);
	switch (command->action) {
	case ResourceActionType::COLLECT: break;
	case ResourceActionType::CANCEL: break;
	default: ;
	}
}

std::string Stats::getOutput(BuildingActionCommand* command) const {
	float output[STATS_OUTPUT_SIZE];
	std::fill_n(output,STATS_OUTPUT_SIZE, 0);
	switch (command->action) {

	case BuildingActionType::UNIT_CREATE: break;
	case BuildingActionType::UNIT_LEVEL: break;
	case BuildingActionType::UNIT_UPGRADE: break;
	default: ;
	}
}
