#include "ActionCenter.h"
#include "commands/action/ActionCommand.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommand.h"
#include "commands/upgrade/UpgradeCommandList.h"
#include "commands/creation/CreationCommandList.h"
#include "Game.h"
#include "stats/Stats.h"

ActionCenter::ActionCenter(SimulationObjectManager* simulationObjectManager) {
	creation = new CreationCommandList(simulationObjectManager);
	upgrade = new UpgradeCommandList(simulationObjectManager);
	action = new CommandList();
}

void ActionCenter::add(UpgradeCommand* command) const {
	Game::getStats()->add(command);
	upgrade->add(command);
}

void ActionCenter::add(CreationCommand* command) const {
	Game::getStats()->add(command);
	creation->add(command);
}

void ActionCenter::add(ActionCommand* command) const {
	Game::getStats()->add(command);
	action->add(command);
}

void ActionCenter::add(ActionCommand* first, ActionCommand* second) const {
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
	return creation->addUnits(number, id, position, player, level);
}

bool ActionCenter::addBuilding(int id, Urho3D::Vector2& position, char player, int level) const {
	return creation->addBuilding(id, position, player, level);
}

auto ActionCenter::addResource(int id, Urho3D::Vector2& position, int level) const -> bool {
	return creation->addResource(id, position, level);
}
