#pragma once
#include "MathUtils.h"
#include "objects/unit/MissileData.h"
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
		unit->missileData->reset();
		unit->missileData->init(*unit->getPosition(),
		                       *unit->thingsToInteract[0]->getPosition(),
		                       unit->thingsToInteract[0]);
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
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
		return sqDist(unit->getPosition(), unit->missileData->aim->getPosition()) < unit->attackRange * unit->attackRange;
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);
		unit->velocity = Urho3D::Vector2::ZERO;
		++unit->currentFrameState;

		if (!unit->missileData->aim || !unit->missileData->aim->isAlive()) {
			unit->missileData->reset();
			StateManager::changeState(unit, UnitState::STOP);
		} else if (!unit->missileData->isUp() && fmod(unit->currentFrameState, 1 / unit->attackSpeed) < 1) {
			if (closeEnough(unit)) {
				shot(unit);
			} else {
				unit->missileData->reset();
				StateManager::changeState(unit, UnitState::STOP);
			}
		}
	}
};
