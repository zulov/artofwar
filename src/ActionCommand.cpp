#include "ActionCommand.h"


ActionCommand::ActionCommand(std::vector<Entity*>* entities, ActionType action, Entity* paremater) {
	this->entities = entities;
	this->action = action;
	this->paremater = paremater;
}


ActionCommand::~ActionCommand() {
}

void ActionCommand::execute() {
	for (int i = 0; i < entities->size(); ++i) {
		(*entities)[i]->action(action, paremater);
	}
}