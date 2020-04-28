#include "AiManager.h"
#include "Game.h"
#include "player/PlayersManager.h"
#include "player/Player.h"

void AiManager::ai() {
	for (auto player : Game::getPlayersMan()->getAllPlayers()) {
		if (Game::getPlayersMan()->getActivePlayer() != player) {
			player->ai();
		}
	}
}
