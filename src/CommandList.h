#pragma once
#include <vector>
#include "AbstractCommand.h"
#include "Entity.h"
#include <vector>

class CommandList
{
public:
	CommandList();
	~CommandList();
	void add(AbstractCommand* command);
	void execute();
	virtual void setParemeters(AbstractCommand* command);
private:
	std::vector<AbstractCommand*>* commands;
};
