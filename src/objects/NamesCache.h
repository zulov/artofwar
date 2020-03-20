#pragma once
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "database/DatabaseCache.h"
#include <Urho3D/Container/Str.h>
#include "queue/QueueActionType.h"


inline Urho3D::String getIconName(ObjectType type, int i) {
	auto dbCache = Game::getDatabase();
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

inline Urho3D::String getIconName(QueueActionType type, int level, int i) {
	DatabaseCache* dbCache = Game::getDatabase();
	switch (type) {
	case QueueActionType::UNIT_CREATE:
		return "unit/" + dbCache->getUnit(i)->icon;
	case QueueActionType::UNIT_LEVEL:
		return "unit/levels/" + Urho3D::String(level) + "/" + dbCache->getUnit(i)->icon;
	case QueueActionType::BUILDING_LEVEL:
		return "building/levels/" + Urho3D::String(level) + "/" + dbCache->getBuilding(i)->icon;
	default:
		return "mock.png";
	}
}

inline Urho3D::String getName(ObjectType type, int i) {
	auto dbCache = Game::getDatabase();
	switch (type) {
	case ObjectType::UNIT:
		return dbCache->getUnit(i)->name;
	case ObjectType::BUILDING:
		return dbCache->getBuilding(i)->name;
	case ObjectType::RESOURCE:
		return dbCache->getResource(i)->name;
	default:
		return "Error";
	}
}
