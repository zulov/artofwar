#pragma once
#include "CommandList.h"
#include "AimContainer.h"


class ActionCommandList :public CommandList
{
public:
	ActionCommandList(AimContainer * _aimContainer);
	~ActionCommandList();
	void setParemeters(AbstractCommand* command) override;
private:
	AimContainer * aimContainer;
};

