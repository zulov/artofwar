#pragma once
#include "State.h"

class MoveState : public State
{
public:
	MoveState();
	~MoveState();
	void onStart(Unit* unit) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

