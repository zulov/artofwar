#pragma once
#include "DatabaseCache.h"
#include "Game.h"
#include "objects/building/BuildingUtils.h"
#include "player/Player.h"
#include "player/PlayersManager.h"

inline db_building_level* getBuildingLevel(char player, int id) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForBuilding(id);
	return Game::getDatabase()->getBuilding(id)->getLevel(level).value(); //TODO bug value
}

inline db_unit_level* getUnitLevel(char player, int id) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForUnit(id);
	return Game::getDatabase()->getUnit(id)->getLevel(level).value(); //TODO bug value
}
