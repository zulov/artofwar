#include "AiManager.h"
#include "Game.h"
#include "player/PlayersManager.h"
#include "player/Player.h"

void AiManager::ai() {
	auto players = Game::getPlayersMan()->getAllPlayers();
	for (auto player : players) {
		if (Game::getPlayersMan()->getActivePlayer() != player) {
			//player->ai();
		}
	}
}
