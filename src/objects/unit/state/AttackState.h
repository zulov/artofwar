#pragma once
#include "State.h"

class AttackState : public State
{
public:
	AttackState();
	~AttackState();
	void onStart(Unit* unit) override;
	void onStart(Unit* unit, ActionParameter* parameter) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

