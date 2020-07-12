#include "PlayersManager.h"
#include "Player.h"
#include "utils/DeleteUtils.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "scene/load/dbload_container.h"
#include "scene/save/SceneSaver.h"


PlayersManager::PlayersManager() = default;


PlayersManager::~PlayersManager() {
	clear_vector(allPlayers);
}

void PlayersManager::load(std::vector<dbload_player*>* players, std::vector<dbload_resource*>* resources) {
	for (auto player : *players) {
		auto newPlayer = new Player(player->nation, player->team, player->id, player->color, player->name,
		                            player->is_active);
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

void PlayersManager::addPlayer(NewGamePlayer& player) {
	auto newPlayer = new Player(player.nation, player.team, player.id, player.color, player.name, player.active);
	if (player.active) {
		activePlayer = newPlayer;
	}
	allPlayers.push_back(newPlayer);
	teams[player.team].push_back(newPlayer);
	newPlayer->setResourceAmount(10000);
}

void PlayersManager::load(NewGameForm* form) {
	for (auto& player : form->players) {
		addPlayer(player);
	}
}

Player* PlayersManager::getActivePlayer() const {
	return activePlayer;
}

Player* PlayersManager::getPlayer(short i) {
	return allPlayers.at(i);
}

std::vector<Player*>& PlayersManager::getTeam(short i) {
	return teams[i];
}

void PlayersManager::changeActive(short i) {
	activePlayer->deactivate();
	activePlayer = allPlayers[i];
	activePlayer->activate();
}

void PlayersManager::save(SceneSaver& saver) {
	saver.savePlayers(allPlayers);
	saver.saveResources(allPlayers);
}

void PlayersManager::update() {
	for (auto player : allPlayers) {
		player->updatePossession();
		player->updateResource();
	}
}

char PlayersManager::getActivePlayerID() {
	return activePlayer->getId();
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
	return allPlayers;//TODO bug id playera a jego index to czêsto nie to samo
}
