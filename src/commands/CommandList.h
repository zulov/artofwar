#pragma once
#include "AbstractCommand.h"
#include <vector>

class CommandList
{
public:
	CommandList();
	virtual ~CommandList();
	void add(AbstractCommand* command);
	void add(AbstractCommand* first, AbstractCommand* second);
	void execute();
protected: 
	virtual void setParemeters(AbstractCommand* command);
private:
	std::vector<AbstractCommand*> commands;
};
