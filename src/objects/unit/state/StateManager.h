#pragma once
#include "State.h"
#include "database/db_strcut.h"
#include "objects/unit/ActionParameter.h"
#include "UnitOrder.h"
#include <array>

class StateManager
{
public:

	void changeState(Unit* unit, UnitState stateTo);
	bool validateState(int id, UnitState stateTo);
	void changeState(Unit* unit, UnitState stateTo, ActionParameter& actionParameter);
	bool checkChangeState(Unit* unit, UnitState stateTo);
	void execute(Unit* unit);
	static StateManager* get();

	static void init();
	static void dispose();
private:
	StateManager();
	~StateManager();
	State* states[STATE_SIZE]{};
	bool ordersToUnit[UNITS_NUMBER_DB][STATE_SIZE];

	static StateManager* instance;

	std::vector<char> orderToState[UNIT_ORDER_SIZE];
};
