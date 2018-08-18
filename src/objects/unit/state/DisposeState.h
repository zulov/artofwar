#pragma once
#include "State.h"

class DisposeState : public State
{
public:
	DisposeState(): State({}) {
	}

	~DisposeState() = default;

	void onStart(Unit* unit, ActionParameter& parameter) override {
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);
	}
};
