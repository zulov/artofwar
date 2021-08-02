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

ActionCenter::ActionCenter(SimulationObjectManager* simulationObjectManager):
	creation(simulationObjectManager), upgrade(simulationObjectManager) {
}

void ActionCenter::add(UpgradeCommand* command) {
	upgrade.add(command);
}

void ActionCenter::add(ResourceActionCommand* command) {
	action.add(command);
}

void ActionCenter::add(BuildingActionCommand* command) {
	Game::getStats()->add(command);
	action.add(command);
}

void ActionCenter::addUnitAction(UnitOrder* first, char player) {
	auto com = new UnitActionCommand(first, player);
	Game::getStats()->add(com);
	action.add(com);
}

void ActionCenter::addUnitAction(UnitOrder* first, UnitOrder* second, char player) {
	auto com1 = new UnitActionCommand(first, player);
	auto com2 = new UnitActionCommand(second, player);
	Game::getStats()->add(com1);
	Game::getStats()->add(com2);
	action.add(com1, com2);
}

void ActionCenter::add(GeneralActionCommand* command) {
	Game::getStats()->add(command);
	action.add(command);
}

void ActionCenter::executeActions() {
	action.execute();
}

void ActionCenter::executeLists() {
	upgrade.execute();
	creation.execute();
}

bool ActionCenter::addUnits(int number, int id, Urho3D::Vector2 position, char player) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForUnit(id)->level;
	return addUnits(number, id, position, player, level);
}

bool ActionCenter::addUnits(int number, int id, Urho3D::Vector2& position, char player, int level) {
	auto command = creation.addUnits(number, id, position, player, level);
	if (command) {
		creation.add(command);
		return true;
	}
	return false;
}

bool ActionCenter::addBuilding(int id, Urho3D::Vector2& position, char player, bool force) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForBuilding(id)->level;
	return addBuilding(id, position, player, level, force);
}

bool ActionCenter::addBuilding(int id, Urho3D::Vector2& position, char player, int level, bool force) {
	CreationCommand* command{};
	if (force) {
		command = creation.addBuildingForce(id, position, player, level);
	} else {
		command = creation.addBuilding(id, position, player, level);
	}

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
