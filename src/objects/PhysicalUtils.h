#pragma once
#include "Physical.h"
#include "database/db_strcut.h"
#include "unit/Unit.h"
#include "unit/state/StateManager.h"

inline bool belowClose(Physical* physical) {
	return physical->belowCloseLimit();
}

inline bool belowRange(Physical* physical) {
	return physical->belowRangeLimit();
}

inline bool isFree(Unit* unit) {
	return unit->getFormation() < 0 && isInFreeState(unit->getState());
}

inline bool isFreeWorker(Unit* unit) {
	return unit->getLevel()->canCollect && isFree(unit);
}

inline bool isFreeSolider(Unit* unit) {
	return !unit->getLevel()->canCollect && isFree(unit);
}
