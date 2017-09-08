#pragma once

class Unit;

class State
{
public:
	State(Unit* _unit);
	~State();
	virtual void onStart() =0;
	virtual void onEnd() =0;
	virtual void execute() =0;
protected:
	Unit* unit;
};
