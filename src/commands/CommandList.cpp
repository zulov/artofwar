#include "CommandList.h"
#include "utils.h"


CommandList::CommandList() = default;


CommandList::~CommandList() {
	clear_vector(commands);
}

void CommandList::add(AbstractCommand* command) {
	commands.push_back(command);
}

void CommandList::add(AbstractCommand* first, AbstractCommand* second) {
	add(first);
	add(second);
}

void CommandList::execute() {
	for (auto& command : commands) {
		command->clean();
		if (!command->expired()) {
			setParameters(command);
			command->execute();
		}
		delete command;
	}
	commands.clear();
}

void CommandList::setParameters(AbstractCommand* command) {

}
