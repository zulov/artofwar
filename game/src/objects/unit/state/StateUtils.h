#pragma once
#include "Game.h"
#include "objects/unit/Unit.h"
#include "env/Environment.h"


inline void setStartData(Unit* unit, Physical* toUse, CellState cellState) {
	unit->setThingToInteract(toUse);
	unit->setIndexToInteract(unit->getMainGridIndex());
	toUse->upClose();

	Game::getEnvironment()->incCell(unit->getMainGridIndex(), cellState);
}

inline void setSlotData(Unit* unit, Unit* toUse) {
	const unsigned char slot = Game::getEnvironment()->getRevertCloseIndex(toUse->getMainGridIndex(), unit->getMainGridIndex());
	unit->setSlotToInteract(slot);
	toUse->setOccupiedIndexSlot(slot, true);
}

inline void reduce(Unit* unit) {
	if (unit->isFirstThingAlive()) {
		unit->getThingToInteract()->reduceClose();
		if (unit->getSlotToInteract() >= 0) {
			unit->getThingToInteract()->setOccupiedIndexSlot(unit->getSlotToInteract(), false);
		}
	}
	if (unit->getIndexToInteract() >= 0) {
		Game::getEnvironment()->decCell(unit->getIndexToInteract());
	}

	unit->resetToInteract();
}
