#pragma once
#include "State.h"
#include "objects/unit/ActionParameter.h"
#include "database/db_strcut.h"


class StateManager
{
public:
	StateManager();
	~StateManager();

	void changeState(Unit* unit, UnitStateType stateTo);
	bool validateState(int getSubTypeId, UnitStateType stateTo);
	void changeState(Unit* unit, UnitStateType stateTo, ActionParameter * actionParameter);
	bool checkChangeState(Unit* unit, UnitStateType stateTo);
	void execute(Unit* unit);
private:
	State** states;

	std::set<UnitStateType> ordersToUnit[UNITS_NUMBER_DB];
};
