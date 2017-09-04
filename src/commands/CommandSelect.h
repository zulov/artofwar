#pragma once
#include "AbstractCommand.h"
#include <utility>
#include "Entity.h"

class CommandSelect :public AbstractCommand
{
public:
	CommandSelect(std::pair<Entity*, Entity*>* _held);
	virtual ~CommandSelect();
	virtual void execute() override;
private:
	std::pair<Entity*, Entity*>* held;
};
