#pragma once
#include "UnitStateType.h"
#include "defines.h"
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
	bool nextStates[STATE_SIZE];
};
