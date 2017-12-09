#pragma once
#include "objects/unit/aim/AimContainer.h"
#include "commands/CommandList.h"


class ActionCommandList :public CommandList
{
public:
	ActionCommandList(AimContainer * _aimContainer);
	~ActionCommandList();
	void setParemeters(AbstractCommand* command) override;
private:
	AimContainer * aimContainer;
};

