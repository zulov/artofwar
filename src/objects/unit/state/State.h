#pragma once
#include "UnitStateType.h"
#include <set>

class Unit;

class State
{
public:
	State();
	~State();
	virtual void onStart(Unit* unit) =0;
	virtual void onEnd(Unit* unit) =0;
	virtual void execute(Unit* unit) =0;
	bool validateTransition(UnitStateType stateTo);
protected:
	std::set<UnitStateType> transitions;
};
