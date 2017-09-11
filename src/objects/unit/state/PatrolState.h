#pragma once
#include "State.h"

class PatrolState : public State
{
public:
	PatrolState();
	~PatrolState();
	void onStart(Unit* unit) override;
	void onStart(Unit* unit, ActionParameter* parameter) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

