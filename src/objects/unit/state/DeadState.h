#pragma once
#include "State.h"

class DeadState : public State
{
public:
	DeadState();
	~DeadState();
	void onStart() override;
	void onEnd() override;
	void execute() override;
};

