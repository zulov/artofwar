#pragma once
#include "AbstractCommand.h"
#include <vector>

class CommandList
{
public:
	CommandList();
	virtual ~CommandList();
	void add(AbstractCommand* command);
	void execute();
protected: 
	virtual void setParemeters(AbstractCommand* command);
private:
	std::vector<AbstractCommand*> commands;
};
