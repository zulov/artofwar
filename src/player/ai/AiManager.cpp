#include "AiManager.h"
#include "Game.h"
#include "player/PlayersManager.h"
#include "simulation/env/Environment.h"

void AiManager::ai() {
	auto players = Game::getPlayersMan()->getAllPlayers();
	for (auto player : players) {
		if (Game::getPlayersMan()->getActivePlayer() != player) {
			//Game::getEnvironment()->
			int a = 5;
		}
	}
}
