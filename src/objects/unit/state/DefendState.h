#pragma once
#include "State.h"

class DefendState : public State
{
public:
	DefendState();
	~DefendState();
	void onStart() override;
	void onEnd() override;
	void execute() override;
};

