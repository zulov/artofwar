#pragma once
#include "Game.h"
#include "QueueActionType.h"
#include "database/DatabaseCache.h"

inline float getSecToComplete(QueueActionType type, unsigned short id) {
	switch (type) {
	case QueueActionType::UNIT_CREATE:
		return Game::getDatabase()->getUnitLevels()[id]->buildTime;
	case QueueActionType::BUILDING_CREATE:
		return Game::getDatabase()->getBuildingLevels()[id]->buildTime;
	case QueueActionType::UNIT_LEVEL:
		return Game::getDatabase()->getUnitLevels()[id]->upgradeTime;
	case QueueActionType::BUILDING_LEVEL:
		return Game::getDatabase()->getBuildingLevels()[id]->upgradeTime;
	case QueueActionType::RESOURCE_CREATE:
		return Game::getDatabase()->getBuildingLevels()[id]->spawnResourceTime;
	case QueueActionType::TECH_RESEARCH:
		return Game::getDatabase()->getTechnologyLevel(id)->researchTime;
	default:
		return 1;
	}
}

inline float getSecPerInstance(QueueActionType type, unsigned short id) {
	switch (type) {
	case QueueActionType::UNIT_CREATE:
		return 30;
	default:
		return 0;
	}
}
