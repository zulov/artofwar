#pragma once
#include "math/MathUtils.h"
#include "objects/unit/MissileData.h"
#include "objects/unit/state/State.h"


class ShotState : public State {
public:
	ShotState() : State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD, UnitState::GO, UnitState::FOLLOW, UnitState::CHARGE
	}) { }

	~ShotState() = default;

	void shot(Unit* unit) {
		unit->missileData->reset();
		unit->missileData->init(unit->getPosition(),
		                        unit->thingToInteract->getPosition(),
		                        unit->thingToInteract);
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		shot(unit);
		unit->thingToInteract->upRange();
		unit->currentFrameState = 1;
	}

	void onEnd(Unit* unit) override {
		if (unit->isFirstThingAlive()) {
			unit->thingToInteract->reduceRange();
			unit->thingToInteract = nullptr;
		}
	}

	bool closeEnough(Unit* unit) const {
		return sqDist(unit->getPosition(), unit->missileData->aim->getPosition()) < unit->dbLevel->sqRangeAttackRange *
			unit->dbLevel->rangeAttackRange;
	}

	void execute(Unit* unit, float timeStep) override {
		unit->velocity = Urho3D::Vector2::ZERO;

		if (!unit->missileData->aim || !unit->missileData->aim->isAlive()) {
			unit->missileData->reset();
			StateManager::toDefaultState(unit);
		} else if (!unit->missileData->isUp() && unit->currentFrameState % unit->dbLevel->rangeAttackReload == 0) {
			if (closeEnough(unit)) {
				shot(unit);
			} else {
				unit->missileData->reset();
				StateManager::toDefaultState(unit);
			}
		}
		++unit->currentFrameState;
	}
};
