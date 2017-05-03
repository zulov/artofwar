#include "SimulationCommand.h"


SimulationCommand::SimulationCommand(std::vector<Entity*>* entities, ActionType action, Entity* paremater) {
	this->entities = entities;
	this->action = action;
	this->paremater = paremater;
}

SimulationCommand::~SimulationCommand() {
}

void SimulationCommand::execute() {
	for (int i = 0; i < entities->size(); ++i) {
		(*entities)[i]->action(action, paremater);
	}
}
