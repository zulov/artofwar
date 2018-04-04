#pragma once
#include "UnitStateType.h"
#include "database/db_strcut.h"
#include "objects/unit/ActionParameter.h"


class Unit;

class State
{
public:
	State();
	virtual ~State();
	virtual void onStart(Unit* unit) =0;
	virtual void onStart(Unit* unit, ActionParameter& parameter) =0;
	virtual void onEnd(Unit* unit);
	virtual void execute(Unit* unit);

	bool validateTransition(UnitStateType stateTo);
protected:
	bool transitions[STATE_SIZE];
};
