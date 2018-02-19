#pragma once
#include "State.h"

class DisposeState : public State
{
public:
	DisposeState();
	~DisposeState();

	void onStart(Unit* unit) override;
	void onStart(Unit* unit, ActionParameter& parameter) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};