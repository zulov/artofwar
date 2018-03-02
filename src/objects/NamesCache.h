#pragma once
#include "Game.h"
#include "ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "queue/QueueType.h"
#include <Urho3D/Container/Str.h>


inline Urho3D::String getIconName(ObjectType type, int i) {
	DatabaseCache* dbCache = Game::get()->getDatabaseCache();
	switch (type) {
	case ObjectType::UNIT:
		return "unit/" + dbCache->getUnit(i)->icon;
	case ObjectType::BUILDING:
		return "building/" + dbCache->getBuilding(i)->icon;
	case ObjectType::RESOURCE:
		return "resource/" + dbCache->getResource(i)->icon;
	default:
		return "mock.png";
	}
}

inline Urho3D::String getIconName(QueueType type, int level, int i) {
	DatabaseCache* dbCache = Game::get()->getDatabaseCache();
	switch (type) {
	case QueueType::UNIT:
		return "unit/" + dbCache->getUnit(i)->icon;
	case QueueType::UNIT_LEVEL:
		return "unit/levels/"+String(level)+"/" + dbCache->getUnit(i)->icon;
	case QueueType::BUILDING_LEVEL:
		return "building/levels/"+String(level)+"/" + dbCache->getBuilding(i)->icon;
	default:
		return "mock.png";
	}
}
