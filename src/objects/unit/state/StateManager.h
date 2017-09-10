#pragma once
#include "State.h"


class StateManager
{
public:
	StateManager();
	~StateManager();

	void changeState(Unit* unit, UnitStateType stateTo);
	bool checkChangeState(Unit* unit, UnitStateType stateTo);
private:
	State** states;
};
