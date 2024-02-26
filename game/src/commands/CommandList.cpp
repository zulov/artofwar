#include "CommandList.h"
#include "PhysicalCommand.h"
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
		command->execute();
		delete command;
	}
	commands.clear();
}
