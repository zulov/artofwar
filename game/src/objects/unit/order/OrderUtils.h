#pragma once
#include "database/db_struct.h"
#include "Game.h"
#include "env/Environment.h"
#include "objects/PhysicalUtils.h"
#include "objects/resource/ResourceEntity.h"

inline bool isAttackAction(UnitAction action) {
	return action == UnitAction::ATTACK || action == UnitAction::CHARGE;
}

inline bool isDefendAction(UnitAction action) {
	return action == UnitAction::DEFEND;
}

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

inline bool toAction(Unit* unit, const std::vector<Physical*>& list, UnitAction order,
                     const std::function<bool(Physical*)>& condition, bool closeEnough) {
	const auto closest = Game::getEnvironment()->closestPhysical(unit->getMainGridIndex(), list, condition, closeEnough);
	return unit->toActionIfInRange(closest, order);
}

inline void tryToAttack(Unit* unit,
                        const std::function<bool(Physical*)>& condition) {
	const bool result = toAction(
		unit, Game::getEnvironment()->getNeighboursFromTeamNotEq(unit, unit->getPosition(), unit->getPlayer(),
		                                                         unit->getLevel()->interestRange),
		UnitAction::ATTACK, condition, true);
	if (!result) {
		toAction(
			unit, Game::getEnvironment()->getBuildingsFromTeamNotEq(unit, unit->getPosition(), unit->getTeam(), -1,
			                                                       unit->getLevel()->interestRange),
			UnitAction::ATTACK, condition, true);
	}
}

inline void tryToCollect(Unit* unit) {
	const auto resourceId = unit->getLastActionThingId();
	if (resourceId < 0) { return; }

	const auto& allResources = Game::getEnvironment()->getResources(unit->getPosition(), unit->getLevel()->interestRange);
	std::vector<Physical*> boosted;
	std::vector<Physical*> regular;
	boosted.reserve(allResources.size());
	regular.reserve(allResources.size());
	for (auto* physical : allResources) {
		if (physical->getSecondaryId() != resourceId) { continue; }
		if (static_cast<ResourceEntity*>(physical)->getBonus(unit->getPlayer()) > 1.f) {
			boosted.push_back(physical);
		} else {
			regular.push_back(physical);
		}
	}

	// Keep gathering the same type. A reachable boosted node wins; otherwise use a regular one.
	if (!toAction(unit, boosted, UnitAction::COLLECT, belowClose, false)) {
		toAction(unit, regular, UnitAction::COLLECT, belowClose, false);
	}
}
