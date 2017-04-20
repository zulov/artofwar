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
	void add(AbstractCommand* command);
	void execute();
private:
	std::vector<AbstractCommand*>* commands;
};
