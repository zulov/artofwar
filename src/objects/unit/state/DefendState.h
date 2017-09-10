#pragma once
#include "State.h"

class DefendState : public State
{
public:
	DefendState();
	~DefendState();
	void onStart(Unit* unit) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

