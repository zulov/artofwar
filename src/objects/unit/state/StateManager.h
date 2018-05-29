#pragma once
#include "State.h"
#include "UnitOrder.h"
#include "database/db_strcut.h"
#include "objects/unit/ActionParameter.h"
#include <array>

class StateManager
{
public:

	bool validateState(int id, UnitState stateTo);
	void changeState(Unit* unit, UnitState stateTo, ActionParameter& actionParameter = ActionParameter());
	bool checkChangeState(Unit* unit, UnitState stateTo);
	void execute(Unit* unit);

	static void init();
	static void dispose();
	static StateManager* get(){return instance;}
private:
	StateManager();
	~StateManager();
	State* states[STATE_SIZE]{};
	bool ordersToUnit[UNITS_NUMBER_DB][STATE_SIZE];
	static StateManager* instance;

	std::vector<char> orderToState[UNIT_ORDER_SIZE];
};
