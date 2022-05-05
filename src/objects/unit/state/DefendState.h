#pragma once
#include "State.h"

class DefendState : public State
{
public:
	DefendState() : State({UnitState::STOP, UnitState::DEAD}) {
	}

	~DefendState() override = default;
};
