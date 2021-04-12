#pragma once
#include "State.h"
#include "../Unit.h"
#include "database/db_strcut.h"
#include "objects/unit/ChargeData.h"

struct ActionParameter;

class ChargeState : public State {
public:
	ChargeState(): State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD, UnitState::GO, UnitState::FOLLOW, UnitState::CHARGE,
		UnitState::MOVE
	}) { }

	~ChargeState() = default;

	void onStart(Unit* unit, const ActionParameter& parameter) override {
		unit->setAim(parameter.aim); //TODO Storzyc charge data tutaj
		// unit->thingsToInteract.clear();
		// unit->thingsToInteract.push_back(parameter.thingToInteract);//TODO to chyba nie potrzebne

		//TODO aim?
		unit->maxSpeed = unit->dbLevel->maxSpeed * 2;
		unit->chargeData->reset();
	}

	void onEnd(Unit* unit) override {
		unit->maxSpeed = unit->dbLevel->maxSpeed;
		unit->removeCurrentAim();
	}

	void execute(Unit* unit, float timeStep) override {
		if (!unit->aims.hasCurrent() && !unit->hasStateChangePending()) {
			StateManager::toDefaultState(unit);
		} else {
			unit->toCharge(
				Game::getEnvironment()->
				getNeighboursFromTeamNotEq(unit, unit->chargeData->attackRange, unit->getTeam()));
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
	}
};
