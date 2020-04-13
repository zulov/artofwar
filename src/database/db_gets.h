#pragma once
#include "DatabaseCache.h"
#include "Game.h"
#include "objects/building/BuildingUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"

db_building_level* getBuildingLevel(char player, int id) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForBuilding(id);
	return Game::getDatabase()->getBuilding(id)->getLevel(level).value(); //TODO bug value
}
