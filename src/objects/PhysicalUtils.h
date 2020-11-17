#pragma once
#include "Physical.h"
#include "unit/Unit.h"
#include "unit/state/StateManager.h"

inline bool belowClose(Physical* physical) {
	return physical->belowCloseLimit();
}

inline bool belowRange(Physical* physical) {
	return physical->belowRangeLimit();
}

inline bool isFreeWorker(Unit* unit) {
	return unit->getLevel()->canCollect && unit->getFormation()<0 && isFree(unit->getState());
}

inline bool isFreeSolider(Unit* unit) {
	return !unit->getLevel()->canCollect && unit->getFormation()<0 && isFree(unit->getState());
}
