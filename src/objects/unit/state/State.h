#pragma once
#include "UnitState.h"


struct ActionParameter;
class Unit;

class State
{
public:
	State();
	virtual ~State();
	virtual bool canStart(Unit* unit);

	virtual void onStart(Unit* unit, ActionParameter& parameter) =0;
	virtual void execute(Unit* unit);
	virtual void onEnd(Unit* unit);

	bool validateTransition(UnitState stateTo);
protected:
	bool nextStates[STATE_SIZE];
};
