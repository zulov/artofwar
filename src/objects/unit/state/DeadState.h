#pragma once
#include "State.h"

class DeadState : public State
{
public:
	DeadState();
	~DeadState();
	void onStart(Unit* unit) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

