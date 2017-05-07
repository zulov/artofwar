#pragma once
#include <vector>
#include "Entity.h"
#include "AbstractCommand.h"

class ActionCommand :public AbstractCommand
{
public:

	ActionCommand(std::vector<Entity*>* entities, ActionType action, Entity* paremater);
	~ActionCommand();
	void execute();
private:
	std::vector<Entity*>* entities;
	ActionType action;
	Entity* paremater;
};

