#pragma once
#include "State.h"
#include "database/db_strcut.h"
#include "objects/building/Building.h"


enum class StaticState : char;
class Static;

class StateManager
{
public:

	static bool validateState(int id, UnitState stateTo);
	static bool changeState(Unit* unit, UnitState stateTo, const ActionParameter& actionParameter);
	static bool changeState(Unit* unit, UnitState stateTo);
	static bool checkChangeState(Unit* unit, UnitState stateTo);
	static void execute(Unit* unit, float timeStamp);

	static bool changeState(Static* obj, StaticState stateTo);
	static void executeChange(Static* obj);

	static void init();
	static void dispose();
private:
	StateManager();
	~StateManager();
	static State* getState(Unit* unit);

	State* states[STATE_SIZE]{};
	
	static StateManager* instance;
};
