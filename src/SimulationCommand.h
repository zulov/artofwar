#pragma once
#include "Entity.h"
#include <vector>
#include "AbstractCommand.h"

class SimulationCommand:public AbstractCommand
{
public:
	SimulationCommand(std::vector<Entity*>* entities, ActionType action, Entity* paremater);
	~SimulationCommand();
	virtual void execute() override;
private:
	std::vector<Entity*>* entities;
	ActionType action;
	Entity* paremater;
};
