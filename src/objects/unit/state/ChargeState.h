#pragma once
#include "../Unit.h"
#include "State.h"
#include "objects/unit/ChargeData.h"
#include "player/Resources.h"


class ChargeState : public State
{
public:
	ChargeState() {
		nextStates[static_cast<char>(UnitStateType::STOP)] = true;
		nextStates[static_cast<char>(UnitStateType::DEFEND)] = true;
		nextStates[static_cast<char>(UnitStateType::DEAD)] = true;
		nextStates[static_cast<char>(UnitStateType::GO)] = true;
		nextStates[static_cast<char>(UnitStateType::FOLLOW)] = true;
		nextStates[static_cast<char>(UnitStateType::CHARAGE)] = true;
		nextStates[static_cast<char>(UnitStateType::MOVE)] = true;
	}

	~ChargeState() = default;

	void onStart(Unit* unit) override {}

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
						StateManager::get()->changeState(unit, UnitStateType::MOVE);
						break;
					}
				}
			}
		} else {
			StateManager::get()->changeState(unit, UnitStateType::MOVE);
		}

	}
};
