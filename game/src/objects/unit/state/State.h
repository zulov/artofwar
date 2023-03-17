#pragma once
#include "UnitState.h"
#include <initializer_list>
#include <magic_enum.hpp>


struct ActionParameter;
class Unit;

class State {
public:
	State(std::initializer_list<UnitState> active);
	virtual ~State() = default;
	virtual bool canStart(Unit* unit, const ActionParameter& parameter);

	virtual void onStart(Unit* unit, const ActionParameter& parameter) {
	}

	virtual void execute(Unit* unit, float timeStep) {
	}

	virtual void onEnd(Unit* unit) {
	}

	bool validateTransition(UnitState stateTo);
protected:
	bool nextStates[magic_enum::enum_count<UnitState>()];
};
