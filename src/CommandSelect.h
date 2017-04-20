#pragma once
#include "Command.h"

class CommandSelect :public AbstractCommand
{
public:
	CommandSelect(std::pair<Entity*, Entity*>* _held);
	~CommandSelect();
	virtual void execute() override;
private:
	std::pair<Entity*, Entity*>* held;
};
