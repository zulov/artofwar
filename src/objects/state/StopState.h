#pragma once
#include "State.h"

class StopState :public State
{
public:
	StopState(Unit* _unit);
	~StopState();
	void onStart() override;
	void onEnd() override;
	void execute() override;
};
