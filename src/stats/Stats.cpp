#include "Stats.h"
#include "Game.h"
#include "player/PlayersManager.h"
#include "player/Player.h"
#include "StatsEnums.h"
#include "simulation/env/Environment.h"
#include "StringUtils.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
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

void Stats::add(UpgradeCommand* command) {
	auto input = getInputFor(command->player);
	std::string data = join(input, input + INPUT_STATS_SIZE);
	std::string output = getOutput(command);
	data.append(output);
	dataToSavePerPlayer[command->player].push_back(data);
}

void Stats::add(ActionCommand* command) {
	auto input = getInputFor(0); //TODO change player
	std::string data = join(input, input + INPUT_STATS_SIZE);
	data.append("Action");
	dataToSavePerPlayer[0].push_back(data);
}

void Stats::add(CreationCommand* command) {
	auto input = getInputFor(command->player);
	std::string data = join(input, input + INPUT_STATS_SIZE);
	data.append("Create");
	dataToSavePerPlayer[command->player].push_back(data);
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
	data[cast(StatsInputType::BASE_TO_ENEMY)] = Game::getEnvironment()->getDistToEnemy(player); //TODO ale do kt�rego
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

std::string Stats::getOutput(UpgradeCommand* command) {
	float output[STATS_OUTPUT_SIZE];
	std::fill_n(output,STATS_OUTPUT_SIZE, 0);
	//TODO command->type;, command->id; wybrac ktore wzmocnic

	output[cast(StatsOutputType::UPGRADE_ATTACK)] = 1;
	output[cast(StatsOutputType::UPGRADE_DEFENCE)] = 1;
	output[cast(StatsOutputType::UPGRADE_ECON)] = 1;
	return join(output, output + STATS_OUTPUT_SIZE);
}