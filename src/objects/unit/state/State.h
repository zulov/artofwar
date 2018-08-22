#pragma once
#include "UnitState.h"
#include <initializer_list>


struct ActionParameter;
class Unit;

class State
{
public:
	State(std::initializer_list<UnitState> active);
	virtual ~State();
	virtual bool canStart(Unit* unit);

	virtual void onStart(Unit* unit, const ActionParameter& parameter) =0;
	virtual void execute(Unit* unit, float timeStep);
	virtual void onEnd(Unit* unit);

	bool validateTransition(UnitState stateTo);
protected:
	bool nextStates[STATE_SIZE];
};
