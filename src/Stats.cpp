#include "Stats.h"
#include "Game.h"
#include "player/PlayersManager.h"
#include "player/Player.h"


void Stats::init() {
	clear();
	for (auto& allPlayer : Game::getPlayersMan()->getAllPlayers()) {
		inputs.push_back(new float[INPUT_STATS_SIZE]);
	}
}

Stats::~Stats() {
	clear();
}

int Stats::getScoreFor(short id) const {
	return Game::getPlayersMan()->getPlayer(id)->getScore();
}

float* Stats::getInputFor(short id) {
	return inputs.at(id);
}

void Stats::clear() {
	clear_vector(inputs);
}
