#pragma once
#include "ActionType.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "database/DatabaseCache.h"
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

inline Urho3D::String getIconName(ActionType type, int level, int i) {
	DatabaseCache* dbCache = Game::get()->getDatabaseCache();
	switch (type) {
	case ActionType::UNIT:
		return "unit/" + dbCache->getUnit(i)->icon;
	case ActionType::UNIT_LEVEL:
		return "unit/levels/" + String(level) + "/" + dbCache->getUnit(i)->icon;
	case ActionType::BUILDING_LEVEL:
		return "building/levels/" + String(level) + "/" + dbCache->getBuilding(i)->icon;
	case ActionType::UNIT_UPGRADE:
		{
		optional<db_unit_upgrade*> opt = dbCache->getUnitUpgrade(i, level);
		if (opt.has_value()) {
			auto upgrade = opt.value();
			return "unit/upgrades/" + upgrade->pathName + "/" + upgrade->name + ".png";
		}
		}
	default:
		return "mock.png";
	}
}
