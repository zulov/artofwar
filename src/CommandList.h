#pragma once
#include <vector>
#include "Command.h"
#include "Entity.h"
#include <vector>

class CommandList
{
public:
	CommandList();
	~CommandList();
	void add(std::vector<Entity*>* entities, ActionType action, Entity* paremater);
	void execute();
private:
	std::vector<Command*> *commands;

};

