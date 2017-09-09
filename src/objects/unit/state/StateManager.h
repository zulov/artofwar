#pragma once
#include "State.h"

class StateManager
{
public:
	StateManager();
	~StateManager();
private:
	State** states;

};

