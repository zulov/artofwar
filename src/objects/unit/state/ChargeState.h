#pragma once
#include "State.h"

class ChargeState : public State
{
public:
	ChargeState();
	~ChargeState();
	void onStart(Unit* unit) override;
	void onStart(Unit* unit, ActionParameter& parameter) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

