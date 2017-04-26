#include "CommandList.h"


CommandList::CommandList() {
	commands = new std::vector<AbstractCommand*>();
}


CommandList::~CommandList() {
	delete commands;
}

void CommandList::add(AbstractCommand * command) {
	commands->push_back(command);
}

void CommandList::execute() {
	for (int i = 0; i < commands->size(); ++i) {
		(*commands)[i]->execute();
		delete (*commands)[i];
	}
	commands->clear();
}
