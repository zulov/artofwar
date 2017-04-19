#include "Command.h"


Command::Command(std::vector<Entity*>* entities, ActionType action, Entity* paremater) {
	this->entities = entities;
	this->action = action;
	this->paremater = paremater;
}

Command::~Command() {
}

void Command::execute() {
	for (int i = 0; i < entities->size(); ++i) {
		(*entities)[i]->action(action, paremater);
	}
}
