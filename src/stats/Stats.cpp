#include "Stats.h"
#include "Game.h"
#include "player/PlayersManager.h"
#include "player/Player.h"
#include "StatsType.h"
#include "simulation/env/Environment.h"


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
}

void Stats::init() {
	clear();
	input = new float[INPUT_STATS_SIZE];
	for (auto& allPlayer : Game::getPlayersMan()->getAllPlayers()) {
		statsPerPlayer.push_back(new float[STATS_PER_PLAYER_SIZE]);
	}
}

void Stats::update(short id) {
	float* data = statsPerPlayer.at(id);
	auto player = Game::getPlayersMan()->getPlayer(id);
	data[cast(StatsType::RESULT)] = player->getScore() / weights[cast(StatsType::RESULT)];
	data[cast(StatsType::GOLD)] = player->getResources().getValues()[0] / weights[cast(StatsType::GOLD)];
	data[cast(StatsType::WOOD)] = player->getResources().getValues()[1] / weights[cast(StatsType::WOOD)];
	data[cast(StatsType::FOOD)] = player->getResources().getValues()[2] / weights[cast(StatsType::FOOD)];
	data[cast(StatsType::STONE)] = player->getResources().getValues()[3] / weights[cast(StatsType::STONE)];
	data[cast(StatsType::ATTACK)] = player->getAttackScore() / weights[cast(StatsType::ATTACK)];
	data[cast(StatsType::DEFENCE)] = player->getDefenceScore() / weights[cast(StatsType::DEFENCE)];
	data[cast(StatsType::BASE_TO_ENEMY)] = Game::getEnvironment()->getDistToEnemy(player); //TODO ale do którego
	data[cast(StatsType::UNITS)] = player->getUnitsNumber();
	data[cast(StatsType::BUILDINGS)] = player->getBuildingsNumber();
	data[cast(StatsType::WORKERS)] = player->getWorkersNumber();
}


int Stats::getScoreFor(short id) const {
	return Game::getPlayersMan()->getPlayer(id)->getScore();
}

float* Stats::getInputFor(short id) {
	update(id);
	auto stats = statsPerPlayer.at(id);
	auto enemy = statsPerPlayer.at(id - 1); //TODO wybrac wroga
	std::copy(stats, stats + STATS_PER_PLAYER_SIZE, input);
	std::copy(enemy, enemy + STATS_PER_PLAYER_SIZE, input + STATS_PER_PLAYER_SIZE);
	return input;
}

void Stats::clear() {
	clear_vector(statsPerPlayer);
	delete []input;
}
