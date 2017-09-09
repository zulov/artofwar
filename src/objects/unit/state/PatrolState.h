#pragma once
#include "State.h"

class PatrolState : public State
{
public:
	PatrolState();
	~PatrolState();
	void onStart() override;
	void onEnd() override;
	void execute() override;
};

