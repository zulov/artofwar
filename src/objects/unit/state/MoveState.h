#pragma once
#include "State.h"

class MoveState : public State
{
public:

	MoveState(): State({
		
		UnitState::DEFEND, UnitState::DEAD,  UnitState::GO,  UnitState::FOLLOW, UnitState::ATTACK,
		UnitState::COLLECT, UnitState::SHOT, UnitState::STOP,UnitState::CHARGE
	}) {
	}

	~MoveState() = default;

};
