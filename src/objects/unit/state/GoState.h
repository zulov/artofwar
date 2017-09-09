#pragma once
#include "State.h"

class GoState : public State
{
public:
	GoState();
	~GoState();
	void onStart() override;
	void onEnd() override;
	void execute() override;
};

