#pragma once
#include "State.h"

class GoState : public State
{
public:
	GoState();
	~GoState();
	void onStart(Unit* unit) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

