#pragma once
#include "State.h"

class ChargeState : public State
{
public:
	ChargeState();
	~ChargeState();
	void onStart() override;
	void onEnd() override;
	void execute() override;
};

