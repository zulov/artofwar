#pragma once
#include "commands/CommandList.h"


class ActionCommandList :public CommandList
{
public:
	ActionCommandList();
	~ActionCommandList();
	void setParemeters(AbstractCommand* command) override;

};

