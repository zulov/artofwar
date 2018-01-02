#pragma once
#include "State.h"

class CollectState : public State
{
public:
	CollectState();
	~CollectState();
	void onStart(Unit* unit) override;
	void onStart(Unit* unit, ActionParameter& parameter) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

