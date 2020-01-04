#include "ActionCenter.h"
#include "commands/action/ActionCommand.h"
#include "commands/creation/CreationCommand.h"
#include "commands/upgrade/UpgradeCommandList.h"
#include "commands/creation/CreationCommandList.h"

ActionCenter::ActionCenter(SimulationObjectManager* simulationObjectManager) {
	creation = new CreationCommandList(simulationObjectManager);
	upgrade = new UpgradeCommandList(simulationObjectManager);
	action = new CommandList();
}

void ActionCenter::add(UpgradeCommand* command) const {
	upgrade->add(command);
}

void ActionCenter::add(CreationCommand* command) const {
	creation->add(command);
}

void ActionCenter::add(ActionCommand* command) const {
	action->add(command);
}

void ActionCenter::add(ActionCommand* first, ActionCommand* second) const {
	action->add(first, second);
}

void ActionCenter::executeActions() const {
	action->execute();
}

void ActionCenter::executeLists() const {
	upgrade->execute();
	creation->execute();
}

bool ActionCenter::addUnits(int number, int id, Urho3D::Vector2& position, int player, int level) const {
	return creation->addUnits(number, id, position, player, level);
}

bool ActionCenter::addBuilding(int id, Urho3D::Vector2& position, int player, int level) const {
	return creation->addBuilding(id, position, player, level);
}

auto ActionCenter::addResource(int id, Urho3D::Vector2& position, int level) -> bool {
	return creation->addResource(id, position, level);
}
