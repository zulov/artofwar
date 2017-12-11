#include "PlayersManager.h"


PlayersManager::PlayersManager() {
}


PlayersManager::~PlayersManager() {
}

void PlayersManager::load(std::vector<dbload_player*>* players, std::vector<dbload_resource*>* resources) {
	for (auto player : *players) {
		Player* newPlayer = new Player(0, player->team, player->id);
		if (player->is_active) {
			activePlayer = newPlayer;
		}
		allPlayers.push_back(newPlayer);
		teams[player->team].push_back(newPlayer);
	}

	for (auto resource : *resources) {
		for (auto player : allPlayers) {
			if (player->getId() == resource->player) {
				player->setResourceAmount(resource->resource, resource->amount);
			}
		}
	}

}

Player* PlayersManager::getActivePlayer() {
	return activePlayer;
}

Player* PlayersManager::getPlayer(short i) {
	return allPlayers.at(i);
}

std::vector<Player*>& PlayersManager::getTeam(short i) {
	return teams[i];
}

void PlayersManager::changeActive(short i) {
	activePlayer = allPlayers[i];
}

void PlayersManager::save(SceneSaver* saver) {
	saver->savePlayers(allPlayers);
	saver->saveResources(allPlayers);
}

std::string PlayersManager::getColumns() {
	return "id		INT     NOT NULL,"
		"is_active		INT     NOT NULL,"
		"team		INT     NOT NULL";
}
