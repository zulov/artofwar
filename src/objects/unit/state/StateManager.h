#pragma once
#include "State.h"
#include "objects/unit/ActionParameter.h"


class StateManager
{
public:
	StateManager();
	~StateManager();

	void changeState(Unit* unit, UnitStateType stateTo);
	void changeState(Unit* unit, UnitStateType stateTo, ActionParameter * actionParameter);
	bool checkChangeState(Unit* unit, UnitStateType stateTo);
	void execute(Unit* unit);
private:
	State** states;
};
