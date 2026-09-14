#include "UpgradeCommandList.h"
#include "UpgradeCommand.h"
#include "scene/load/RuntimeSaveData.h"


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

std::vector<PendingCommandSaveData> UpgradeCommandList::saveState(unsigned short& nextOrder) const {
	std::vector<PendingCommandSaveData> state;
	state.reserve(commands.size());
	for (const auto* command : commands) {
		state.push_back(command->saveState(nextOrder++));
	}
	return state;
}
