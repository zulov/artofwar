#pragma once
#include "State.h"


class StopState : public State {
public:

	StopState(): State({

		UnitState::DEFEND, UnitState::DEAD, UnitState::GO, UnitState::FOLLOW, UnitState::ATTACK,
		UnitState::COLLECT, UnitState::SHOT, UnitState::MOVE, UnitState::CHARGE
	}) {
	}

	~StopState() override = default;

};
