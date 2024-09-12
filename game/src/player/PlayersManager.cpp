#include "PlayersManager.h"
#include "Player.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "scene/load/dbload_container.h"
#include "scene/save/SceneSaver.h"
#include "simulation/PerFrameAction.h"


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

	for (const auto resource : *resources) {
		for (const auto player : allPlayers) {
			if (player->getId() == resource->player) {
				player->setResourceAmount(resource->food, resource->wood,resource->stone, resource->gold);
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
	activePlayer = allPlayers[i];
}

void PlayersManager::save(SceneSaver& saver) {
	saver.savePlayers(allPlayers);
	saver.saveResources(allPlayers);
}

void PlayersManager::update(FrameInfo* frameInfo) {
	for (const auto player : allPlayers) {
		player->updatePossession();
		if (PER_FRAME_ACTION.get(PerFrameAction::RESOURCE_GATHER_SPEED, frameInfo)) {
			player->updateResource1s();
		}

		if (PER_FRAME_ACTION.get(PerFrameAction::RESOURCE_MONTH_UPDATE, frameInfo)) {
			player->updateResourceMonth();
		}

		if (PER_FRAME_ACTION.get(PerFrameAction::RESOURCE_YEAR_UPDATE, frameInfo)) {
			player->updateResourceYear();
		}

		player->resetScore();
	}
}

char PlayersManager::getActivePlayerID() const {
	return activePlayer->getId();
}

char PlayersManager::getEnemyIdFor(char player) {
	if (player == 0) {
		//TODO bug do it better
		return 1;
	}
	return 0;
}

Player* PlayersManager::getEnemyFor(char player) const {
	return allPlayers[getEnemyIdFor(player)];
}

std::vector<Player*>& PlayersManager::getAllPlayers() {
	return allPlayers; //TODO bug id playera a jego index to czêsto nie to samo
}
