#include "PlayersManager.h"
#include "Game.h"
#include "Player.h"
#include "Resources.h"
#include "database/DatabaseCache.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "objects/UId.h"
#include "scene/load/dbload_container.h"
#include "scene/save/SceneSaver.h"
#include "simulation/FrameInfo.h"


PlayersManager::~PlayersManager() {
	clear_vector(allPlayers);
}

void PlayersManager::load(const std::vector<dbload_player*>* players) {
	for (auto player : *players) {
		const auto colors = Game::getDatabase()->getPlayerColors().size();
		const auto color = static_cast<unsigned char>(colors ? player->id % colors : 0);
		const auto name = Urho3D::String("Player ") + Urho3D::String(static_cast<int>(player->id));
		auto newPlayer = new Player(player->nation, player->team, player->id, color, name,
		                            player->is_active, player->buildingUid, player->unitUid);
		newPlayer->setResourceAmount(player->food, player->wood, player->stone, player->gold);
		newPlayer->getResources()->loadState(player->resources);
		if (player->is_active) {
			activePlayer = newPlayer;
		}
		allPlayers.push_back(newPlayer);
		teams[player->team].push_back(newPlayer);
	}
}

void PlayersManager::addPlayer(NewGamePlayer& player) {
	const auto newPlayer = new Player(
		player.nation, player.team, player.id,
		player.color, player.name, player.active,
		UId::create(ObjectType::BUILDING, player.id),
		UId::create(ObjectType::UNIT, player.id)
	);
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

Player* PlayersManager::getPlayer(unsigned char i) const{
	return allPlayers.at(i);
}

std::vector<Player*>& PlayersManager::getTeam(short i) {
	return teams[i];
}

void PlayersManager::changeActive(unsigned char i) {
	activePlayer = allPlayers[i];
}

void PlayersManager::update(FrameInfo* frameInfo) {
	for (const auto player : allPlayers) {
		player->updatePossession();
		if (frameInfo->shouldRun(PerFrameAction::RESOURCE_GATHER_SPEED)) {
			player->updateResource1s();
		}

		if (frameInfo->shouldRun(PerFrameAction::RESOURCE_MONTH_UPDATE)) {
			player->updateResourceMonth();
		}

		if (frameInfo->shouldRun(PerFrameAction::RESOURCE_YEAR_UPDATE)) {
			player->updateResourceYear();
		}

		player->resetScore();
	}
}

unsigned char PlayersManager::getActivePlayerID() const {
	return activePlayer->getId();
}

void PlayersManager::activateNext() { activePlayer = getEnemyFor(activePlayer->getId()); }

unsigned char PlayersManager::getEnemyIdFor(unsigned char player) const {
	if (player == 0) {
		//TODO bug do it better
		return 1;
	}
	return 0;
}

Player* PlayersManager::getEnemyFor(unsigned char player) const {
	return allPlayers[getEnemyIdFor(player)];
}

std::vector<Player*>& PlayersManager::getAllPlayers() {
	return allPlayers; //TODO bug id playera a jego index to cz�sto nie to samo
}
