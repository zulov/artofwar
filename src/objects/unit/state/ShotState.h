#pragma once
#include "objects/unit/MissleData.h"
#include "objects/unit/state/State.h"
#include "MathUtils.h"


class ShotState : public State
{
public:
	ShotState() {
		nextStates[static_cast<char>(UnitState::STOP)] = true;
		nextStates[static_cast<char>(UnitState::DEFEND)] = true;
		nextStates[static_cast<char>(UnitState::DEAD)] = true;
		nextStates[static_cast<char>(UnitState::GO_TO)] = true;
		nextStates[static_cast<char>(UnitState::FOLLOW)] = true;
		nextStates[static_cast<char>(UnitState::CHARGE)] = true;
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
