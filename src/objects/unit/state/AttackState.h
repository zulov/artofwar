#pragma once
#include "State.h"

class AttackState : public State
{
public:
	AttackState();
	~AttackState();
	void onStart() override;
	void onEnd() override;
	void execute() override;
};

