#pragma once
#include "Physical.h"
#include "database/db_strcut.h"
#include "unit/Unit.h"
#include "unit/state/StateManager.h"

inline bool alwaysFalse(Physical* physical) {
	return false;
}

inline bool belowClose(Physical* physical) {
	return physical->belowCloseLimit();
}

inline bool belowRange(Physical* physical) {
	return physical->belowRangeLimit();
}

inline bool belowCloseOrRange(Physical* physical) {
	return physical->belowRangeLimit() || physical->belowCloseLimit();
}

inline bool isFree(Unit* unit) {
	return unit->getFormation() < 0 && isInFreeState(unit->getState()) && !unit->hasAim();
}

inline bool isWorker(Physical* physical) {
	return ((Unit*)physical)->getDbUnit()->typeWorker;
}

inline bool isFreeWorker(Unit* unit) {
	return unit->getDbUnit()->typeWorker && isFree(unit);
}

inline bool isFreeSolider(Unit* unit) {
	return !unit->getDbUnit()->typeWorker && isFree(unit);
}

inline float getCircleSize(const db_building* dbBuilding, const db_building_level* level) {
	if (dbBuilding->typeDefence) {
		return level->attackRange;
	}
	if (dbBuilding->typeResourceFood || dbBuilding->typeResourceWood
		|| dbBuilding->typeResourceStone || dbBuilding->typeResourceGold) {
		return level->resourceRange;
	}
	return level->sightRadius;
}
