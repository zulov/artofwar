#pragma once
#include "Game.h"
#include "objects/unit/Unit.h"
#include "env/Environment.h"


inline void setStartData(Unit* unit, int index, Physical* toUse) {
	unit->setThingToInteract(toUse);
	unit->setIndexToInteract(index);
	toUse->upClose();
}

inline void setSlotData(Unit* unit, int index, Unit* toUse) {
	const char slot = Game::getEnvironment()->getRevertCloseIndex(toUse->getMainGridIndex(), index);
	unit->setSlotToInteract(slot);
	toUse->setOccupiedIndexSlot(slot, true);
}
