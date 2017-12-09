#include "CommandList.h"
#include "utils.h"


CommandList::CommandList() {
	commands = new std::vector<AbstractCommand*>();
}


CommandList::~CommandList() {
	clear_and_delete_vector(commands);
}

void CommandList::add(AbstractCommand* command) {
	commands->push_back(command);
}

void CommandList::execute() {
	for (int i = 0; i < commands->size(); ++i) {
		setParemeters((*commands)[i]);
		(*commands)[i]->execute();
		delete (*commands)[i];
	}
	commands->clear();
}

void CommandList::setParemeters(AbstractCommand* command) {

}
