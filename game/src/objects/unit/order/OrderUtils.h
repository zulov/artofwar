#pragma once
#include "database/db_strcut.h"
#include "Game.h"
#include "env/Environment.h"
#include "objects/PhysicalUtils.h"

template <typename T>
void removeExpired(std::vector<T*>& orders) {
	orders.erase(std::remove_if(orders.begin(), orders.end(),
	                            [](T* uo) {
		                            const bool expired = uo->expired();
		                            if (expired) {
			                            delete uo;
		                            }
		                            return expired;
	                            }),
	             orders.end());
}

inline bool toAction(Unit* unit, std::vector<Physical*>* list, UnitAction order,
                     const std::function<bool(Physical*)>& condition) {
	const auto closest = Game::getEnvironment()->closestPhysical(unit, list, condition,
		unit->getLevel()->sqSightRadius);
	return unit->toActionIfInRange(closest, order);
}

inline void tryToAttack(Unit* unit,
                        const std::function<bool(Physical*)>& condition) {
	const bool result = toAction(
		unit, Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, unit->getLevel()->interestRange),
		UnitAction::ATTACK, condition);
	if (!result) {
		toAction(
			unit, Game::getEnvironment()->getBuildingsFromTeamNotEq(unit, -1, unit->getLevel()->interestRange),
			UnitAction::ATTACK, condition);
	}
}

inline void tryToCollect(Unit* unit) {
	const auto id = unit->getLastActionThingId();
	std::vector<Physical*>* list;

	bool result = false;
	if (id >= 0) {
		list = Game::getEnvironment()->getResources(unit->getPosition(), id, unit->getLevel()->interestRange);
		result = toAction(unit, list, UnitAction::COLLECT, belowClose);
	}
	if (id < 0 || !result) {
		list = Game::getEnvironment()->getResources(unit->getPosition(), -1, unit->getLevel()->interestRange);
		toAction(unit, list, UnitAction::COLLECT, belowClose);
	}
}
