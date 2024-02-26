#include "UpgradeCommandList.h"
#include "UpgradeCommand.h"


UpgradeCommandList::UpgradeCommandList(SimulationObjectManager* simulationObjectManager)
	: simulationObjectManager(simulationObjectManager) {}

void UpgradeCommandList::add(UpgradeCommand* command) {
	commands.push_back(command);
}

void UpgradeCommandList::execute() {
	for (const auto command : commands) {
		command->execute(simulationObjectManager);
		delete command;
	}
	commands.clear();
}
