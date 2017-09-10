#pragma once
#include "State.h"

class AttackState : public State
{
public:
	AttackState();
	~AttackState();
	void onStart(Unit* unit) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

