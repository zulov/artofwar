#pragma once

#include "database/db_struct.h"
#include "database/db_technology_struct.h"
#include "player/Player.h"

namespace TechnologyUtils {

inline bool matchesUnitTag(TechnologySourceTag tag, const db_unit* unit) {
	switch (tag) {
	case TechnologySourceTag::ANY: return true;
	case TechnologySourceTag::ARMY: return !unit->typeWorker;
	case TechnologySourceTag::WORKER: return unit->typeWorker;
	case TechnologySourceTag::INFANTRY: return unit->typeInfantry;
	case TechnologySourceTag::RANGED: return unit->typeRange;
	case TechnologySourceTag::CAVALRY: return unit->typeCavalry;
	case TechnologySourceTag::MELEE: return unit->typeMelee;
	case TechnologySourceTag::HEAVY: return unit->typeHeavy;
	case TechnologySourceTag::LIGHT: return unit->typeLight;
	case TechnologySourceTag::SPECIAL: return unit->typeSpecial;
	default: return false;
	}
}

inline bool matchesBuildingTag(TechnologySourceTag tag, const db_building* building) {
	return tag == TechnologySourceTag::ANY || tag == TechnologySourceTag::ALL_BUILDINGS ||
		(tag == TechnologySourceTag::RESOURCE_BUILDING && building->isResourceBuilding()) ||
		(tag == TechnologySourceTag::DEFENSIVE_BUILDING && building->typeDefence) ||
		(tag == TechnologySourceTag::TECH_BUILDING && building->parentType[static_cast<int>(ParentBuildingType::TECH)]);
}

inline bool matchesTargetId(short targetId, unsigned short id) {
	return targetId < 0 || static_cast<unsigned short>(targetId) == id;
}

inline bool matchesAttackTarget(const db_technology_effect* effect, const db_unit* target) {
	return effect->targetKind == TechnologyTargetKind::UNIT && matchesTargetId(effect->targetId, target->id) &&
		matchesUnitTag(effect->targetTag, target);
}

inline bool matchesAttackTarget(const db_technology_effect* effect, const db_building* target) {
	return effect->targetKind == TechnologyTargetKind::BUILDING && matchesTargetId(effect->targetId, target->id) &&
		matchesBuildingTag(effect->targetTag, target);
}

inline bool matchesResourceTarget(const db_technology_effect* effect, unsigned char resourceId) {
	if (effect->targetKind != TechnologyTargetKind::RESOURCE || !matchesTargetId(effect->targetId, resourceId)) {
		return false;
	}
	const auto resourceType = lowerTechnologyName(effect->resourceType.c_str());
	return resourceType.empty() || resourceType == "none" || resourceType == "any" ||
		(resourceType == "food" && resourceId == cast(ResourceType::FOOD)) ||
		(resourceType == "wood" && resourceId == cast(ResourceType::WOOD)) ||
		(resourceType == "stone" && resourceId == cast(ResourceType::STONE)) ||
		(resourceType == "gold" && resourceId == cast(ResourceType::GOLD));
}

inline bool isActive(const Player* player, const db_technology_level* technology) {
	return technology && technology->technology < player->getTechnologyLevels().size() &&
		player->getTechnologyLevel(technology->technology) >= technology->level;
}

inline void applyEffect(float& value, const db_technology_effect* effect) {
	value = effect->operation == TechnologyOperation::PERCENT ? value * (1.f + effect->value) : value + effect->value;
}

}
