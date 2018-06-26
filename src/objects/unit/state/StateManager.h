#pragma once
#include "State.h"
#include "UnitOrder.h"
#include "database/db_strcut.h"
#include "objects/unit/ActionParameter.h"

class StateManager
{
public:

	static bool validateState(int id, UnitState stateTo);
	static bool changeState(Unit* unit, UnitState stateTo, ActionParameter& actionParameter = ActionParameter());
	static bool checkChangeState(Unit* unit, UnitState stateTo);
	static void execute(Unit* unit);

	static void init();
	static void dispose();
private:
	StateManager();
	~StateManager();
	static State* getState(Unit* unit);

	State* states[STATE_SIZE]{};
	bool ordersToUnit[UNITS_NUMBER_DB][STATE_SIZE];
	static StateManager* instance;

	std::vector<char> orderToState[UNIT_ORDER_SIZE];
};
