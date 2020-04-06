#include "ActionCenter.h"
#include "Game.h"
#include "stats/Stats.h"
#include "player/PlayersManager.h"
#include "player/Player.h"
#include "database/DatabaseCache.h"
#include "commands/action/UnitActionCommand.h"
#include "commands/action/ResourceActionCommand.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "commands/upgrade/UpgradeCommandList.h"
#include "commands/creation/CreationCommandList.h"

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

bool ActionCenter::addUnits(int number, int id, Urho3D::Vector2& position, char player, int level) const {
	auto command = creation->addUnits(number, id, position, player, level);
	if (command) {
		//Game::getStats()->add(command);
		creation->add(command);
		return true;
	}
	return false;
}

bool ActionCenter::addBuilding(int id, Urho3D::Vector2& position, char player, int level) const {
	auto command = creation->addBuilding(id, position, player, level);
	if (command) {
		Game::getStats()->add(command);//TODO to dodaæ przez z jak¹œ kolejkê
		creation->add(command);
		return true;
	}
	return false;
}

auto ActionCenter::addResource(int id, Urho3D::Vector2& position, int level) const -> bool {
	auto command = creation->addResource(id, position, level);
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
		const auto level = player->getLevelForBuilding(id) + 1;
		auto opt = Game::getDatabase()->getBuilding(id)->getLevel(level);
		if (opt.has_value()) {		
			if (player->getResources().reduce(opt.value()->costs)) {
				player->getQueue().add(1, QueueActionType::BUILDING_LEVEL, id, 1);
			}
		}
	}
}
