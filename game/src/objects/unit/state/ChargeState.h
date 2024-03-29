#pragma once
#include "State.h"
#include "../Unit.h"
#include "database/db_strcut.h"
#include "math/MathUtils.h"
#include "objects/unit/ChargeData.h"

struct ActionParameter;

class ChargeState : public State {
public:
	ChargeState(): State({
		UnitState::STOP, UnitState::DEFEND, UnitState::DEAD, UnitState::GO, UnitState::FOLLOW, UnitState::CHARGE,
		UnitState::MOVE
	}) {
	}

	~ChargeState() = default;

	bool canStart(Unit* unit, const ActionParameter& parameter) override {
		return parameter.aim != nullptr;
	}

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
			const auto thingsToInteract = toCharge(
				Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, unit->chargeData->attackRange), unit);
			if (unit->chargeData->updateFrame()) {
				for (const auto physical : thingsToInteract) {
					if (unit->getTeam() != physical->getTeam()) {
						const auto before = physical->getHealthPercent();

						const auto [value, died] = physical->absorbAttack(
							unit->getAttackVal(physical) / thingsToInteract.size());//TODO mo�e nie dzieli� uzale�nic od czego�?
						Game::getEnvironment()->addAttack(unit->getPlayer(), physical->getPosition(), value);
						if (died) {
							Game::getPlayersMan()->getPlayer(unit->getPlayer())->addKilled(physical);
						}

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

	std::vector<Physical*> toCharge(const std::vector<Physical*>* enemies, Unit* unit) const {
		std::vector<Physical*> thingsToInteract;
		const auto sqRange = unit->chargeData->attackRange * unit->chargeData->attackRange;
		for (auto entity : *enemies) {
			if (entity->isAlive() && sqDist(unit->getPosition(), entity->getPosition()) <= sqRange) {
				thingsToInteract.push_back(entity);
			}
		}
		return thingsToInteract;
	}
};
