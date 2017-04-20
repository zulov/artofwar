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
	void add(Command* command);
	void execute();
private:
	std::vector<Command*>* commands;
};
