#pragma once
#include "UnitStateType.h"
#include <set>
#include "objects/unit/ActionParameter.h"

class Unit;

class State
{
public:
	State();
	~State();
	virtual void onStart(Unit* unit) =0;
	virtual void onStart(Unit* unit, ActionParameter* parameter) =0;
	virtual void onEnd(Unit* unit) =0;
	virtual void execute(Unit* unit) =0;
	bool validateTransition(UnitStateType stateTo);
protected:
	std::set<UnitStateType> transitions;
};
