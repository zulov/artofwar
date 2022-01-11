#pragma once
#include "math/MathUtils.h"
#include "objects/projectile/ProjectileManager.h"
#include "objects/unit/state/State.h"


class ShotState : public State {
public:
	ShotState() : State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD, UnitState::GO, UnitState::FOLLOW, UnitState::CHARGE
	}) {
	}

	~ShotState() = default;

	void shot(Unit* unit) {
		ProjectileManager::shoot(unit, unit->thingToInteract, 7, unit->getPlayer());
		unit->currentFrameState = 0;
	}

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		assert(unit->getLevel()->typeRange);
		if (parameter.isFirstThingAlive()) {
			auto const indexesToUse = parameter.thingToInteract->getIndexesForRangeUse(unit);
			return std::ranges::find(indexesToUse, unit->getMainGridIndex()) != indexesToUse.end()
				&& parameter.thingToInteract->belowRangeLimit() > 0;
		}
		return false;
	}

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->thingToInteract = parameter.thingToInteract;
		shot(unit);
		unit->thingToInteract->upRange();
		unit->currentFrameState = 0;
	}

	void onEnd(Unit* unit) override {
		if (unit->isFirstThingAlive()) {
			unit->thingToInteract->reduceRange();
			unit->thingToInteract = nullptr;
		}
	}

	bool closeEnough(Unit* unit) const {
		return sqDist(unit->getPosition(), unit->thingToInteract->getPosition()) < unit->dbLevel->sqAttackRange;
	}

	void execute(Unit* unit, float timeStep) override {
		//unit->velocity = Urho3D::Vector2::ZERO;

		if (!unit->thingToInteract || !unit->thingToInteract->isAlive()) {
			StateManager::toDefaultState(unit);
		} else if (unit->currentFrameState >= unit->dbLevel->attackReload) {
			if (closeEnough(unit)) {
				//TODO tu cos innego
				shot(unit);
			} else {
				StateManager::toDefaultState(unit);
			}
		}
		++unit->currentFrameState;
	}
};
