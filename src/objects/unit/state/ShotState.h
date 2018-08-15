#pragma once
#include "MathUtils.h"
#include "objects/unit/MissleData.h"
#include "objects/unit/state/State.h"


class ShotState : public State
{
public:
	ShotState() : State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD, UnitState::GO_TO, UnitState::FOLLOW, UnitState::CHARGE
	}) {
	}

	~ShotState() = default;

	void shot(Unit* unit) {
		unit->missleData->reset();
		unit->missleData->init(*unit->getPosition(),
		                       *unit->thingsToInteract[0]->getPosition(),
		                       unit->thingsToInteract[0]);
	}

	void onStart(Unit* unit, ActionParameter& parameter) override {
		shot(unit);
		unit->thingsToInteract[0]->upRange();
		unit->currentFrameState = 0;
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceRange();
			unit->thingsToInteract.clear();
		}
		unit->currentFrameState = 0;
	}

	bool closeEnough(Unit* unit) {
		return sqDist(unit, unit->missleData->aim) < unit->attackRange * unit->attackRange;
	}

	void execute(Unit* unit) override {
		State::execute(unit);
		unit->velocity = Urho3D::Vector2::ZERO;
		++unit->currentFrameState;

		if (!unit->missleData->aim || !unit->missleData->aim->isAlive()) {
			unit->missleData->reset();
			StateManager::changeState(unit, UnitState::STOP);
		} else if (!unit->missleData->isUp() && fmod(unit->currentFrameState, 1 / unit->attackSpeed) < 1) {
			if (closeEnough(unit)) {
				shot(unit);
			} else {
				unit->missleData->reset();
				StateManager::changeState(unit, UnitState::STOP);
			}
		}
	}
};
