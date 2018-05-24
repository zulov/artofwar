#pragma once
#include "objects/unit/state/State.h"

class ShotState : public State
{
public:
	ShotState();
	~ShotState();

	void onStart(Unit* unit) override {
	}

	void onStart(Unit* unit, ActionParameter& parameter) override {
	}

	void onEnd(Unit* unit) {
		State::onEnd(unit);
	}

	void execute(Unit* unit) {
		State::execute(unit);
	}
};
