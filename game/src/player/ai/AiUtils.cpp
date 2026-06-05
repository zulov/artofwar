#include "AiUtils.h"

#include "database/db_struct.h"
#include "objects/building/ParentBuildingType.h"
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
