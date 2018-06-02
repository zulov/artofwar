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

	void onStart(Unit* unit, ActionParameter& parameter) override {
		unit->missleData->init(*unit->getPosition(),
		                       *unit->thingsToInteract[0]->getPosition(),
		                       unit->thingsToInteract[0]);
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->upRange();
		}
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		if (unit->isFirstThingAlive()) {
			unit->thingsToInteract[0]->reduceRange();
		}
	}

	void execute(Unit* unit) override {
		State::execute(unit);
		unit->velocity = Urho3D::Vector2::ZERO;
		//if aim is but no misle then shot again
		if (unit->missleData->finished()) {
			//TODO to nie powinno byc tu tylko w missle, atu tylko strzelanie
			if (unit->missleData->aim && unit->missleData->aim->isAlive()) {
				auto distAttack = sqDist(*unit->missleData->aim->getPosition(), unit->missleData->getPosition());
				if (distAttack < 3 * 3) {
					unit->missleData->aim->absorbAttack(unit->attackCoef);
				}
			}
			unit->missleData->reset();
			StateManager::get()->changeState(unit, UnitState::STOP);
			return;
		}

		if (!unit->missleData->aim || !unit->missleData->aim->isAlive()) {
			unit->missleData->reset();
			StateManager::get()->changeState(unit, UnitState::STOP);
		}
	}
};
