#pragma once
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "database/DatabaseCache.h"
#include <Urho3D/Container/Str.h>
#include "queue/QueueActionType.h"


inline Urho3D::String getIconName(ObjectType type, unsigned short id) {
	auto dbCache = Game::getDatabase();
	switch (type) {
	case ObjectType::UNIT:
		return "unit/" + dbCache->getUnit(id)->icon;
	case ObjectType::BUILDING:
		return "building/" + dbCache->getBuilding(id)->icon;
	case ObjectType::RESOURCE:
		return "resource/" + dbCache->getResource(id)->icon;
	default:
		return "mock.png";
	}
}

inline Urho3D::String getIconName(QueueActionType type,unsigned short id) {
	DatabaseCache* dbCache = Game::getDatabase();
	switch (type) {
	case QueueActionType::UNIT_CREATE:
		return "unit/" + dbCache->getUnit(id)->icon;
	case QueueActionType::UNIT_LEVEL:
		return "unit/levels/" + Urho3D::String(0) + "/" + dbCache->getUnit(id)->icon;
	case QueueActionType::BUILDING_LEVEL:
		return "building/levels/" + Urho3D::String(0) + "/" + dbCache->getBuilding(id)->icon;
	case QueueActionType::BUILDING_CREATE:
		return "building/" + dbCache->getBuilding(id)->icon;	
	case QueueActionType::RESOURCE_CREATE:
		return "mock.png";
		//"resources/" + dbCache->getResource(i)->icon;
	case QueueActionType::TECH_RESEARCH:
		return "technology/" + dbCache->getTechnologyLevel(id)->icon;
	default:
		return "mock.png";
	}
}
