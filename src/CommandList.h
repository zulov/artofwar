#pragma once
#include <vector>
#include "Command.h"

class CommandList
{
public:
	CommandList();
	~CommandList();
private:
	std::vector<Command*> commands;

};

