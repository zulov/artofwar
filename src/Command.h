#pragma once
#include "Entity.h"
#include <vector>

class Command
{
public:
	Command(std::vector<Entity*>* entities, ActionType action, Entity* paremater);
	~Command();
	void execute();
private:
	std::vector<Entity*>* entities;
	ActionType action;
	Entity* paremater;
};

