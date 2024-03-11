#pragma once
#include "database/db_strcut.h"
#include "Game.h"
#include "env/Environment.h"
#include "objects/PhysicalUtils.h"

template <typename T>
void removeExpired(std::vector<T*>& orders) {
	//TODO use std::stable partition
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
                     const std::function<bool(Physical*)>& condition, bool closeEnough) {
	const auto closest = Game::getEnvironment()->closestPhysical(unit->getMainGridIndex(), list, condition, closeEnough);
	return unit->toActionIfInRange(closest, order);
}

inline void tryToAttack(Unit* unit,
                        const std::function<bool(Physical*)>& condition) {
	const bool result = toAction(
		unit, Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, unit->getLevel()->interestRange),
		UnitAction::ATTACK, condition, true);
	if (!result) {
		toAction(
			unit, Game::getEnvironment()->getBuildingsFromTeamNotEq(unit, -1, unit->getLevel()->interestRange),
			UnitAction::ATTACK, condition, true);
	}
}

inline void tryToCollect(Unit* unit) {
	const auto id = unit->getLastActionThingId();
	std::vector<Physical*>* allResources = Game::getEnvironment()->getResources(unit->getPosition(), unit->getLevel()->interestRange);

	bool result = false;
	if (id >= 0) {
		std::vector<Physical*> resWithId;
		for (const auto physical : *allResources) {
			if (physical->getSecondaryId() == id) {
				resWithId.push_back(physical);
			}
		}
		result = toAction(unit, &resWithId, UnitAction::COLLECT, belowClose, false);
	}
	if (!result) {
		toAction(unit, allResources, UnitAction::COLLECT, belowClose, false);
	}
}
