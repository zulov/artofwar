#include "AiManager.h"
#include "Game.h"
#include "player/PlayersManager.h"
#include "player/Player.h"

void AiManager::aiOrder() {
	for (auto player : Game::getPlayersMan()->getAllPlayers()) {
		if (Game::getPlayersMan()->getActivePlayer() != player) {
			player->aiOrder();
		}
	}
}

void AiManager::aiAction() {
	for (auto player : Game::getPlayersMan()->getAllPlayers()) {
		if (Game::getPlayersMan()->getActivePlayer() != player) {
			player->aiAction();
		}
	}
}
