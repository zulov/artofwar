#pragma once
#include "State.h"
#include "../Unit.h"
#include "database/db_strcut.h"
#include "objects/unit/ChargeData.h"

struct ActionParameter;

class ChargeState : public State {
public:
	ChargeState(): State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD, UnitState::GO_TO, UnitState::FOLLOW, UnitState::CHARGE,
		UnitState::MOVE
	}) {
	}

	~ChargeState() = default;

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->setAim(parameter.aim);
		unit->thingsToInteract.clear();
		unit->thingsToInteract = parameter.thingsToInteract;

		//TODO aim?
		unit->maxSpeed = unit->dbLevel->maxSpeed * 2;
		unit->chargeData->reset();
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		unit->maxSpeed = unit->dbLevel->maxSpeed;
	}

	void execute(Unit* unit, float timeStep) override {
		State::execute(unit, timeStep);

		if (unit->chargeData->updateFrame()) {
			for (auto physical : unit->thingsToInteract) {
				if (unit->getTeam() != physical->getTeam()) {
					const auto before = physical->getHealthPercent();
					physical->absorbAttack(unit->dbLevel->chargeAttackVal);
					const auto after = physical->getHealthPercent();
					if (!unit->chargeData->updateHit(before, after)) {
						StateManager::changeState(unit, UnitState::MOVE);
						break;
					}
				}
			}
		} else {
			StateManager::changeState(unit, UnitState::MOVE);
		}

	}
};
