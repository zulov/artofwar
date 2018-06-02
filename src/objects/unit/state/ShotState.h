#pragma once
#include "objects/unit/MissleData.h"
#include "objects/unit/state/State.h"


class ShotState : public State
{
public:
	ShotState() {
	}

	~ShotState() = default;

	void onStart(Unit* unit, ActionParameter& parameter) override {
		unit->missleData->init(*unit->getPosition(),
		                       *unit->thingsToInteract[0]->getPosition(),
		                       nullptr);
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
	}

	void execute(Unit* unit) override {
		State::execute(unit);
		if (unit->isFirstThingAlive() || !unit->missleData->finished()) {
			unit->velocity = Urho3D::Vector2::ZERO;
		} else {
			unit->thingsToInteract.clear();
			StateManager::get()->changeState(unit, UnitState::STOP);
		}
	}
};
