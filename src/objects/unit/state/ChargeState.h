#pragma once
#include "../Unit.h"
#include "State.h"
#include "objects/unit/ChargeData.h"
#include "player/Resources.h"


class ChargeState : public State
{
public:
	ChargeState() {
		nextStates[static_cast<char>(UnitState::STOP)] = true;
		nextStates[static_cast<char>(UnitState::DEFEND)] = true;
		nextStates[static_cast<char>(UnitState::DEAD)] = true;
		nextStates[static_cast<char>(UnitState::GO_TO)] = true;
		nextStates[static_cast<char>(UnitState::FOLLOW)] = true;
		nextStates[static_cast<char>(UnitState::CHARGE)] = true;
		nextStates[static_cast<char>(UnitState::MOVE)] = true;
	}

	~ChargeState() = default;

	void onStart(Unit* unit, ActionParameter& parameter) override {
		unit->addAim(parameter.aim);
		//TODO aim?
		unit->maxSpeed = unit->dbLevel->maxSpeed * 2;
		unit->chargeData->reset();
	}

	void onEnd(Unit* unit) override {
		State::onEnd(unit);
		unit->maxSpeed = unit->dbLevel->maxSpeed;
	}

	void execute(Unit* unit) override {
		State::execute(unit);

		if (unit->chargeData->updateFrame()) {
			for (auto physical : unit->thingsToInteract) {
				if (unit->getTeam() != physical->getTeam()) {
					auto before = physical->getHealthPercent();
					physical->absorbAttack(unit->attackCoef);
					auto after = physical->getHealthPercent();
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
