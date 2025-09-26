#include "player/ai/ActionCenter.h"
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/ResourceActionCommand.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/UnitActionCommand.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "env/Environment.h"
#include "player/Player.h"
#include "player/PlayersManager.h"

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
	action.add(command);
}

void ActionCenter::addUnitAction(UnitOrder* first) {
	action.add(new UnitActionCommand(first));
}

void ActionCenter::addUnitAction(UnitOrder* first, UnitOrder* second) {
	action.add(new UnitActionCommand(first), new UnitActionCommand(second));
}

void ActionCenter::add(GeneralActionCommand* command) {
	action.add(command);
}

void ActionCenter::executeActions() {
	action.execute();
}

void ActionCenter::executeLists() {
	upgrade.execute();
	creation.execute();
}

bool ActionCenter::addUnits(unsigned number, short id, Urho3D::Vector2 position, char player) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForUnit(id)->level;
	return addUnits(number, id, position, player, level);
}

bool ActionCenter::addUnits(unsigned number, short id, Urho3D::Vector2& position, char player, int level) {
	auto command = creation.addUnits(number, id, position, player, level);
	if (command) {
		creation.add(command);
		return true;
	}
	return false;
}

bool ActionCenter::addBuilding(short id, Urho3D::Vector2& position, char player, bool force) {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForBuilding(id)->level;
	return addBuilding(id, position, player, level, force);
}

bool ActionCenter::addBuilding(short id, Urho3D::Vector2& position, char player, int level, bool force) {
	CreationCommand* command{};
	if (force) {
		command = creation.addBuildingForce(id, position, player, level);
	} else {
		command = creation.addBuilding(id, position, player, level);
	}

	if (command) {
		creation.add(command);
		return true;
	}
	return false;
}

bool ActionCenter::addResource(short id, int index, float hp) {
	auto cords = Game::getEnvironment()->getCords(index);
	const auto command = creation.addResource(id, cords);
	if (command) {
		command->setHp(hp);
		creation.add(command);
		return true;
	}

	return false;
}
