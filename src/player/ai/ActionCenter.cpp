#include "player/ai/ActionCenter.h"
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/ResourceActionCommand.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/UnitActionCommand.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "stats/Stats.h"

ActionCenter::ActionCenter(SimulationObjectManager* simulationObjectManager): creation(simulationObjectManager),
                                                                              upgrade(simulationObjectManager) {
}

void ActionCenter::add(UpgradeCommand* command) {
	//Game::getStats()->add(command); //TODO BUG to nie jest akcja tylko rezultat
	upgrade.add(command);
}

void ActionCenter::add(ResourceActionCommand* command) {
	//Game::getStats()->add(command);
	action.add(command);
}


void ActionCenter::add(BuildingActionCommand* command) {
	Game::getStats()->add(command);
	action.add(command);
}

void ActionCenter::add(UnitActionCommand* command) {
	Game::getStats()->add(command);
	action.add(command);
}

void ActionCenter::add(GeneralActionCommand* command) {
	Game::getStats()->add(command);
	action.add(command);
}

void ActionCenter::add(UnitActionCommand* first, UnitActionCommand* second) {
	Game::getStats()->add(first);
	Game::getStats()->add(second);
	action.add(first, second);
}

void ActionCenter::executeActions() {
	action.execute();
}

void ActionCenter::executeLists() {
	upgrade.execute();
	creation.execute();
}

bool ActionCenter::addUnits(int number, int id, Urho3D::Vector2& position, char player) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForUnit(id)->level;
	return addUnits(number, id, position, player, level);
}

bool ActionCenter::addUnits(int number, int id, Urho3D::Vector2& position, char player, int level) {
	auto command = creation.addUnits(number, id, position, player, level);
	if (command) {
		//Game::getStats()->add(command);
		creation.add(command);
		return true;
	}
	return false;
}


bool ActionCenter::addBuilding(int id, Urho3D::Vector2& position, char player) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForBuilding(id)->level;
	return addBuilding(id, position, player, level);
}

bool ActionCenter::addBuilding(int id, Urho3D::Vector2& position, char player, int level) {
	auto command = creation.addBuilding(id, position, player, level);
	if (command) {
		Game::getStats()->add(command); //TODO to dodaæ przez z jak¹œ kolejkê
		creation.add(command);
		return true;
	}
	return false;
}

bool ActionCenter::addResource(int id, Urho3D::Vector2& position) {
	auto command = creation.addResource(id, position, 0);
	if (command) {
		creation.add(command);
		return true;
	}

	return false;
}
