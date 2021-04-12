#pragma once
#include "math/MathUtils.h"
#include "objects/unit/MissileData.h"
#include "objects/unit/state/State.h"


class ShotState : public State
{
public:
	ShotState() : State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD, UnitState::GO, UnitState::FOLLOW, UnitState::CHARGE
	}) {
	}

	~ShotState() = default;

	void shot(Unit* unit) {
		unit->missileData->reset();
		unit->missileData->init(unit->getPosition(),
		                       unit->thingsToInteract[0]->getPosition(),
		                       unit->thingsToInteract[0]);
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		shot(unit);
		unit->thingsToInteract[0]->upRange();
		unit->currentFrameState = 0;
	}

	void onEnd(Unit* unit) override {
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceRange();
			unit->thingsToInteract.clear();
		}
		unit->currentFrameState = 0;
	}

	bool closeEnough(Unit* unit) const {
		return sqDist(unit->getPosition(), unit->missileData->aim->getPosition()) < unit->dbLevel->sqRangeAttackRange * unit->dbLevel->rangeAttackRange;
	}

	void execute(Unit* unit, float timeStep) override {
		unit->velocity = Urho3D::Vector2::ZERO;
		++unit->currentFrameState;

		if (!unit->missileData->aim || !unit->missileData->aim->isAlive()) {
			unit->missileData->reset();
			StateManager::toDefaultState(unit);
		} else if (!unit->missileData->isUp() && fmod(unit->currentFrameState, 1 / unit->dbLevel->rangeAttackSpeed) < 1) {
			if (closeEnough(unit)) {
				shot(unit);
			} else {
				unit->missileData->reset();
				StateManager::toDefaultState(unit);
			}
		}
	}
};
