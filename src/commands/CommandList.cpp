#include "CommandList.h"
#include "utils.h"


CommandList::CommandList() = default;


CommandList::~CommandList() {
	clear_vector(commands);
}

void CommandList::add(AbstractCommand* command) {
	commands.push_back(command);
}

void CommandList::execute() {
	for (auto & command : commands) {
		setParemeters(command);
		command->execute();
		delete command;
	}
	commands.clear();
}

void CommandList::setParemeters(AbstractCommand* command) {

}
