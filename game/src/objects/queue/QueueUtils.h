#pragma once
#include "Game.h"
#include "QueueActionType.h"
#include "database/DatabaseCache.h"

inline float getSecToComplete(QueueActionType type, short id, short levelId) {
	switch (type) {
	case QueueActionType::UNIT_CREATE:
		return Game::getDatabase()->getUnitLevels()[levelId]->buildTime;
	case QueueActionType::BUILDING_CREATE:
		return Game::getDatabase()->getBuildingLevels()[levelId]->buildTime;
	case QueueActionType::UNIT_LEVEL:
		return Game::getDatabase()->getUnitLevels()[levelId]->upgradeTime;
	case QueueActionType::BUILDING_LEVEL:
		return Game::getDatabase()->getBuildingLevels()[levelId]->upgradeTime;
	case QueueActionType::RESOURCE_CREATE:
		return Game::getDatabase()->getBuildingLevels()[levelId]->spawnResourceTime;
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
	case QueueActionType::RESOURCE_CREATE:
	default:
		return 0;
	}
}