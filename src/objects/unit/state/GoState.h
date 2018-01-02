#pragma once
#include "State.h"

class GoState : public State
{
public:
	GoState();
	~GoState();
	void onStart(Unit* unit) override;
	void onStart(Unit* unit, ActionParameter& parameter) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

