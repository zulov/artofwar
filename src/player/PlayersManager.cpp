#include "PlayersManager.h"


PlayersManager::PlayersManager() = default;


PlayersManager::~PlayersManager() {
	clear_vector(allPlayers);
}

void PlayersManager::load(std::vector<dbload_player*>* players, std::vector<dbload_resource*>* resources) {
	for (auto player : *players) {
		Player* newPlayer = new Player(player->nation, player->team, player->id, player->color, player->name);
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

void PlayersManager::addPlayer(NewGamePlayer player) {
	Player* newPlayer = new Player(player.nation, player.team, player.id, player.color, player.name);
	if (player.active) {
		activePlayer = newPlayer;
	}
	allPlayers.push_back(newPlayer);
	teams[player.team].push_back(newPlayer);
	newPlayer->setResourceAmount(10000);
}

void PlayersManager::load(NewGameForm* form) {
	for (const auto& player : form->players) {
		addPlayer(player);
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

void PlayersManager::save(SceneSaver& saver) {
	saver.savePlayers(allPlayers);
	saver.saveResources(allPlayers);
}

std::string PlayersManager::getColumns() {
	return "id		INT     NOT NULL,"
		"is_active		INT     NOT NULL,"
		"team		INT     NOT NULL,"
		"nation		INT     NOT NULL,"
		"name		TEXT     NOT NULL,"
		"color		INT     NOT NULL";
}

std::vector<Player*>& PlayersManager::getAllPlayers() {
	return allPlayers;
}
