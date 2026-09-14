#include "CommandList.h"
#include "PhysicalCommand.h"
#include "scene/load/RuntimeSaveData.h"
#include "utils/DeleteUtils.h"

CommandList::~CommandList() {
	clear_vector(commands);
}

void CommandList::add(PhysicalCommand* command) {
	commands.push_back(command);
}

void CommandList::add(PhysicalCommand* first, PhysicalCommand* second) {
	add(first);
	add(second);
}

void CommandList::execute() {
	for (const auto command : commands) {
		//if(!command->expired()) {//TODO error
			command->execute();
		//}
		
		delete command;
	}
	commands.clear();
}

std::vector<PendingCommandSaveData> CommandList::saveState(unsigned short& nextOrder) const {
	std::vector<PendingCommandSaveData> state;
	state.reserve(commands.size());
	for (const auto* command : commands) {
		state.push_back(command->saveState(nextOrder++));
	}
	return state;
}
