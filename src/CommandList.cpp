#include "CommandList.h"


CommandList::CommandList() {
	commands = new std::vector<Command*>();
}


CommandList::~CommandList() {
}

void CommandList::add(Command * command) {

	commands->push_back(command);
}

void CommandList::execute() {
	for (int i = 0; i < commands->size(); ++i) {
		(*commands)[i]->execute();
		delete (*commands)[i];
	}
	commands->clear();
}
