#pragma once
#include "CommandList.h"

class AimContainer;

class ActionCommandList :public CommandList
{
public:
	ActionCommandList(AimContainer * _aimContainer);
	~ActionCommandList();
	void setParemeters(AbstractCommand* command);
private:
	AimContainer * aimContainer;
};

