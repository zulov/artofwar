#include "AiUtils.h"

#include "database/db_struct.h"
#include "objects/building/ParentBuildingType.h"
#include "objects/building/BuildPlacementClass.h"
#include "player/Player.h"

float avgUnitLevel(const std::vector<db_unit*>& units, Player* player,
                   const std::function<bool(const db_unit*)>& filter) {
	float sum = 0.f;
	int count = 0;
	for (auto* unit : units) {
		if (filter(unit) && !unit->levels.empty()) {
			float maxLevel = static_cast<float>(unit->levels.size() - 1);
			if (maxLevel > 0.f) {
				sum += static_cast<float>(player->getLevelForUnit(unit->id)->level) / maxLevel;
				++count;
			}
		}
	}
	return count > 0 ? sum / static_cast<float>(count) : 0.f;
}

float avgBuildingLevel(const std::vector<db_building*>& buildings, Player* player,
                       ParentBuildingType type) {
	float sum = 0.f;
	int count = 0;
	for (auto* building : buildings) {
		if (building->parentType[static_cast<int>(type)] && !building->levels.empty()) {
			float maxLevel = static_cast<float>(building->levels.size() - 1);
			if (maxLevel > 0.f) {
				sum += static_cast<float>(player->getLevelForBuilding(building->id)->level) / maxLevel;
				++count;
			}
		}
	}
	return count > 0 ? sum / static_cast<float>(count) : 0.f;
}

BuildPlacementClass classifyPlacement(const db_building* building, const db_building_level* level) {
	// Resource subtypes first, most specific wins.
	if (building->typeResourceAny && level->collect > 0.f && level->resourceRange > 0.f) {
		return BuildPlacementClass::RES_BONUS;
	}
	if (building->toResource >= 0 && level->spawnResourceRange > 0) {
		return BuildPlacementClass::RES_SPAWNER;
	}
	if (building->toResource >= 0) {
		return BuildPlacementClass::RES_CONVERT;
	}
	if (level->stoneRefineCapacity > 0.f || level->goldRefineCapacity > 0.f) {
		return BuildPlacementClass::RES_REFINE;
	}
	if (level->foodStorage > 0 || level->goldStorage > 0) {
		return BuildPlacementClass::RES_STORAGE;
	}

	// Non-resource: map from ParentBuildingType (first set flag).
	if (building->parentType[static_cast<int>(ParentBuildingType::DEFENCE)]) {
		return BuildPlacementClass::DEFENCE;
	}
	if (building->parentType[static_cast<int>(ParentBuildingType::TECH)]) {
		return BuildPlacementClass::TECH;
	}
	if (building->parentType[static_cast<int>(ParentBuildingType::UNITS)]) {
		return BuildPlacementClass::UNITS;
	}
	return BuildPlacementClass::OTHER;
}

float avgTechLevel(const std::vector<db_unit*>& units, const std::vector<db_building*>& buildings, Player* player) {
	float sum = 0.f;
	int count = 0;

	for (auto* unit : units) {
		if (!unit->levels.empty()) {
			float maxLevel = static_cast<float>(unit->levels.size() - 1);
			if (maxLevel > 0.f) {
				sum += static_cast<float>(player->getLevelForUnit(unit->id)->level) / maxLevel;
				++count;
			}
		}
	}

	for (auto* building : buildings) {
		if (!building->levels.empty()) {
			float maxLevel = static_cast<float>(building->levels.size() - 1);
			if (maxLevel > 0.f) {
				sum += static_cast<float>(player->getLevelForBuilding(building->id)->level) / maxLevel;
				++count;
			}
		}
	}

	return count > 0 ? sum / static_cast<float>(count) : 0.f;
}
