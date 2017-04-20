#pragma once
#include "Entity.h"
#include <vector>
#include "AbstractCommand.h"

class Command:public AbstractCommand
{
public:
	Command(std::vector<Entity*>* entities, ActionType action, Entity* paremater);
	~Command();
	virtual void execute() override;
private:
	std::vector<Entity*>* entities;
	ActionType action;
	Entity* paremater;
};
