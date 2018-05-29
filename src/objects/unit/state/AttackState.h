#pragma once
#include "State.h"
#include "StateManager.h"
#include "objects/unit/Unit.h"

class AttackState : public State
{
public:
	AttackState() {
		nextStates[static_cast<char>(UnitState::STOP)] = true;
		nextStates[static_cast<char>(UnitState::DEFEND)] = true;
		nextStates[static_cast<char>(UnitState::DEAD)] = true;
		nextStates[static_cast<char>(UnitState::GO)] = true;
		nextStates[static_cast<char>(UnitState::FOLLOW)] = true;
		nextStates[static_cast<char>(UnitState::CHARGE)] = true;
	}


	~AttackState() = default;

	void onStart(Unit* unit) override {
		unit->velocity = Urho3D::Vector2::ZERO;
		unit->currentFrameState = 0;
	}

	void onStart(Unit* unit, ActionParameter& parameter) override {}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		unit->thingsToInteract.clear();
	}

	void execute(Unit* unit) override {
		State::execute(unit);
		if (unit->isFirstThingAlive()) {
			unit->velocity = Urho3D::Vector2::ZERO;
			if (fmod(unit->currentFrameState, 1 / unit->attackSpeed) < 1) {
				unit->thingsToInteract[0]->absorbAttack(unit->attackCoef);
			}
			++unit->currentFrameState;
		} else {
			unit->thingsToInteract.clear();
			StateManager::get()->changeState(unit, UnitState::STOP);
		}
	}
};
