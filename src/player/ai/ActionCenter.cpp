#include "player/ai/ActionCenter.h"
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/ResourceActionCommand.h"
#include "commands/action/UnitActionCommand.h"
#include "commands/creation/CreationCommand.h"
#include "commands/creation/CreationCommandList.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "commands/upgrade/UpgradeCommandList.h"
#include "database/DatabaseCache.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "stats/Stats.h"

ActionCenter::ActionCenter(SimulationObjectManager* simulationObjectManager) {
	creation = new CreationCommandList(simulationObjectManager);
	upgrade = new UpgradeCommandList(simulationObjectManager);
	action = new CommandList();
}

void ActionCenter::add(UpgradeCommand* command) const {
	//Game::getStats()->add(command); //TODO BUG to nie jest akcja tylko rezultat
	upgrade->add(command);
}

void ActionCenter::add(ResourceActionCommand* command) const {
	Game::getStats()->add(command);
	action->add(command);
}


void ActionCenter::add(BuildingActionCommand* command) const {
	Game::getStats()->add(command);
	action->add(command);
}

void ActionCenter::add(UnitActionCommand* command) const {
	Game::getStats()->add(command);
	action->add(command);
}

void ActionCenter::add(UnitActionCommand* first, UnitActionCommand* second) const {
	Game::getStats()->add(first);
	Game::getStats()->add(second);
	action->add(first, second);
}

void ActionCenter::executeActions() const {
	action->execute();
}

void ActionCenter::executeLists() const {
	upgrade->execute();
	creation->execute();
}

bool ActionCenter::addUnits(int number, int id, Urho3D::Vector2& position, char player) const {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForUnit(id)->level;
	return addUnits(number, id, position, player, level);
}

bool ActionCenter::addUnits(int number, int id, Urho3D::Vector2& position, char player, int level) const {
	auto command = creation->addUnits(number, id, position, player, level);
	if (command) {
		//Game::getStats()->add(command);
		creation->add(command);
		return true;
	}
	return false;
}


bool ActionCenter::addBuilding(int id, Urho3D::Vector2& position, char player) const {
	auto level = Game::getPlayersMan()->getPlayer(player)->getLevelForBuilding(id)->level;
	return addBuilding(id, position, player, level);
}

bool ActionCenter::addBuilding(int id, Urho3D::Vector2& position, char player, int level) const {
	auto command = creation->addBuilding(id, position, player, level);
	if (command) {
		Game::getStats()->add(command); //TODO to doda� przez z jak�� kolejk�
		creation->add(command);
		return true;
	}
	return false;
}

auto ActionCenter::addResource(int id, Urho3D::Vector2& position) const -> bool {
	auto command = creation->addResource(id, position, 0);
	if (command) {
		//Game::getStats()->add(command);
		creation->add(command);
		return true;
	}

	return false;
}

void ActionCenter::orderPhysical(short id, QueueActionType type, char playerId) const {
	if (type == QueueActionType::BUILDING_LEVEL) {
		Game::getStats()->addBuildLevel(id, playerId);
		auto player = Game::getPlayersMan()->getPlayer(playerId);

		auto opt = player->getNextLevelForBuilding(id);
		if (opt.has_value()) {
			if (player->getResources().reduce(opt.value()->costs)) {
				player->getQueue().add(1, QueueActionType::BUILDING_LEVEL, id, 1);
			}
		}
	}
}