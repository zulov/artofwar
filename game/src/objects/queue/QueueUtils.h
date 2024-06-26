#pragma once
#include "Game.h"
#include "QueueActionType.h"
#include "database/DatabaseCache.h"

inline float getSecToComplete(QueueActionType type, short id, int level) {
	switch (type) {
	case QueueActionType::UNIT_CREATE:
		return Game::getDatabase()->getUnit(id)->getLevel(level).value()->buildTime;
	case QueueActionType::BUILDING_CREATE:
		return Game::getDatabase()->getBuilding(id)->getLevel(level).value()->buildTime;
	case QueueActionType::UNIT_LEVEL:
		return Game::getDatabase()->getUnit(id)->getLevel(level).value()->upgradeTime; //TODO BUG value
	case QueueActionType::BUILDING_LEVEL:
		return Game::getDatabase()->getBuilding(id)->getLevel(level).value()->upgradeTime;
	default:
		return 1;
	}
}

inline float getSecPerInstance(QueueActionType type, short id, int level) {
	//TODO performance przerobic na tablice
	switch (type) {
	case QueueActionType::UNIT_CREATE:
		return 30;
	case QueueActionType::BUILDING_CREATE:
	case QueueActionType::UNIT_LEVEL:
	case QueueActionType::BUILDING_LEVEL:
	default:
		return 0;
	}
}