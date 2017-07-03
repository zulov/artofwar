#pragma once
#include "State.h"

class StopState :public State
{
public:
	StopState();
	~StopState();
	void onStart() override;
	void onEnd() override;
	void execute() override;
};
