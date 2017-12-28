#include "CommandList.h"
#include "utils.h"


CommandList::CommandList() {
}


CommandList::~CommandList() {
	clear_vector(commands);
}

void CommandList::add(AbstractCommand* command) {
	commands.push_back(command);
}

void CommandList::execute() {
	for (int i = 0; i < commands.size(); ++i) {
		setParemeters(commands[i]);
		commands[i]->execute();
		delete commands[i];
	}
	commands.clear();
}

void CommandList::setParemeters(AbstractCommand* command) {

}
