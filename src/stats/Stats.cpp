#include "Stats.h"
#include "Game.h"
#include "player/PlayersManager.h"
#include "player/Player.h"
#include "StatsType.h"
#include "simulation/env/Environment.h"
#include "StringUtils.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include <fstream>


Stats::Stats() {
	weights[cast(StatsType::RESULT)] = 1000;
	weights[cast(StatsType::GOLD)] = 1000;
	weights[cast(StatsType::WOOD)] = 1000;
	weights[cast(StatsType::FOOD)] = 1000;
	weights[cast(StatsType::STONE)] = 1000;
	weights[cast(StatsType::ATTACK)] = 1000;
	weights[cast(StatsType::DEFENCE)] = 1000;
	weights[cast(StatsType::BASE_TO_ENEMY)] = 1000;
	weights[cast(StatsType::UNITS)] = 1000;
	weights[cast(StatsType::BUILDINGS)] = 100;
	weights[cast(StatsType::WORKERS)] = 100;
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
	std::string data = join(input, input + INPUT_STATS_SIZE, ';');
	data.append("Upgrade");
	dataToSave.push_back(data);
}

void Stats::add(ActionCommand* command) {
	auto input = getInputFor(0); //TODO change player
	std::string data = join(input, input + INPUT_STATS_SIZE, ';');
	data.append("Action");
	dataToSave.push_back(data);
}

void Stats::add(CreationCommand* command) {
	auto input = getInputFor(command->player);
	std::string data = join(input, input + INPUT_STATS_SIZE, ';');
	data.append("Create");
	dataToSave.push_back(data);
}

void Stats::save() {
	if (dataToSave.size() >= SAVE_BATCH_SIZE) {
		std::ofstream outFile;
		outFile.open("Data/ai/test.txt", std::ios_base::app);
		for (const auto& e : dataToSave) {
			outFile << e << "\n";
		}
		outFile.close();
		dataToSave.clear();
	}
}

void Stats::update(short id) {
	float* data = statsPerPlayer.at(id);
	auto player = Game::getPlayersMan()->getPlayer(id);
	data[cast(StatsType::RESULT)] = player->getScore();
	data[cast(StatsType::GOLD)] = player->getResources().getValues()[0];
	data[cast(StatsType::WOOD)] = player->getResources().getValues()[1];
	data[cast(StatsType::FOOD)] = player->getResources().getValues()[2];
	data[cast(StatsType::STONE)] = player->getResources().getValues()[3];
	data[cast(StatsType::ATTACK)] = player->getAttackScore();
	data[cast(StatsType::DEFENCE)] = player->getDefenceScore();
	data[cast(StatsType::BASE_TO_ENEMY)] = Game::getEnvironment()->getDistToEnemy(player); //TODO ale do którego
	data[cast(StatsType::UNITS)] = player->getUnitsNumber();
	data[cast(StatsType::BUILDINGS)] = player->getBuildingsNumber();
	data[cast(StatsType::WORKERS)] = player->getWorkersNumber();
	std::transform(data, data + STATS_PER_PLAYER_SIZE, weights, data, std::divides<>());
}


int Stats::getScoreFor(short id) const {
	return Game::getPlayersMan()->getPlayer(id)->getScore();
}

float* Stats::getInputFor(short id) {
	update(id);
	auto stats = statsPerPlayer.at(id);
	char idEnemy; //TODO do it better
	if (id == 0) {
		idEnemy = 1;
	} else {
		idEnemy = 0;
	}
	auto enemy = statsPerPlayer.at(idEnemy); //TODO BUG wybrac wroga
	std::copy(stats, stats + STATS_PER_PLAYER_SIZE, input);
	std::copy(enemy, enemy + STATS_PER_PLAYER_SIZE, input + STATS_PER_PLAYER_SIZE);
	return input;
}

void Stats::clear() {
	clear_vector(statsPerPlayer);
}
