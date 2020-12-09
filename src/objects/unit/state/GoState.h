#pragma once
#include "../Unit.h"
#include "State.h"


class GoState : public State {
public:
	GoState() : State({
		UnitState::STOP, UnitState::MOVE, UnitState::DEFEND,
		UnitState::DEAD, UnitState::GO_TO, UnitState::FOLLOW,
		UnitState::CHARGE
	}) {
	}

	~GoState() = default;

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->setAim(parameter.aim);
		unit->thingsToInteract.clear();
	}

	void onEnd(Unit* unit) override {
		//TODO probable bug czy czyscic, ale moze przjsceie do mova zaltwia sprawe
	}

	void execute(Unit* unit, float timeStep) override {
		//TODO probable bug trzeba wyjsc jesli nie ma juz aima
	}
};
