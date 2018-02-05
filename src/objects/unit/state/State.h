#pragma once
#include "UnitStateType.h"
#include "objects/unit/ActionParameter.h"
#include <set>

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
	std::set<UnitStateType> transitions;
};
