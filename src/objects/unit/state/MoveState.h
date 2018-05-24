#pragma once
#include "State.h"

class MoveState : public State
{
public:

	MoveState() {
		nextStates[static_cast<char>(UnitStateType::STOP)] = true;
		nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
		nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
		nextStates[static_cast<char>(UnitStateType::GO)] = true;
		nextStates[static_cast<char>(UnitStateType::ATTACK)] = true;
		nextStates[static_cast<char>(UnitStateType::PATROL)] = true;
		nextStates[static_cast<char>(UnitStateType::FOLLOW)] = true;
		nextStates[static_cast<char>(UnitStateType::CHARAGE)] = true;
		nextStates[static_cast<char>(UnitStateType::COLLECT)] = true;
	}


	~MoveState() = default;

	void onStart(Unit* unit) {}

	void onStart(Unit* unit, ActionParameter& parameter) {}

	void onEnd(Unit* unit) {
		State::onEnd(unit);
	}

	void execute(Unit* unit) {
		State::execute(unit);
	}
};
