#include "PlayersManager.h"


PlayersManager::PlayersManager() {
	activePlayer = new Player(0, 0, 0);
	allPlayers.push_back(activePlayer);
	Player* enemy = new Player(0, 1, 1);
	allPlayers.push_back(enemy);
	teams[0] = new std::vector<Player*>();
	teams[1] = new std::vector<Player*>();
	teams[0]->push_back(activePlayer);
	teams[1]->push_back(enemy);
}


PlayersManager::~PlayersManager() {
}

Player* PlayersManager::getActivePlayer() {
	return activePlayer;
}

Player* PlayersManager::getPlayer(short i) {
	return allPlayers.at(i);
}

std::vector<Player*>* PlayersManager::getTeam(short i) {
	return teams[i];
}

void PlayersManager::changeActive(short i) {
	activePlayer = allPlayers[i];
}

void PlayersManager::save(SceneSaver* saver) {
	saver->savePlayers(allPlayers);
	saver->saverResources(allPlayers);
}

std::string PlayersManager::getColumns() {
	return "id		INT     NOT NULL,"
		"is_active		INT     NOT NULL";
}
