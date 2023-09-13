#pragma once
#include "State.h"

class DefendState : public State
{
public:
	DefendState() : State({UnitState::STOP, UnitState::DEAD}) {
	}

	~DefendState() override = default;

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->resetFormation();
	}
};
