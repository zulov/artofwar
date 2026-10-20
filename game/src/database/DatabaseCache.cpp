#include "DatabaseCache.h"

#include "db_grah_structs.h"
#include "db_other_struct.h"
#include "db_world_age_struct.h"
#include "db_update_utils.h"
#include "db_utils.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <algorithm>

namespace {
[[noreturn]] void exitDatabaseError(const std::string& message) {
	std::cerr << "[Database Error] " << message << "\n";
	std::exit(EXIT_FAILURE);
}

}

bool DatabaseCache::openDatabase(const std::string& name, bool readOnly) {
	database = openDb(pathStr + name, readOnly);
	return database;
}

DatabaseCache::DatabaseCache() {
	container = new db_container();
	database = nullptr;

	pathStr = std::string("Data/");
	if (!SIM_GLOBALS.HEADLESS) { loadBasic("Database/base.db"); }

	loadData("Database/data.db");
	loadMaps("map/maps.db");
}

void DatabaseCache::loadBasic(const std::string& name) {
	if (!openDatabase(name)) { exitDatabaseError("Required data database failed to open: " + name); }

	load<HudSizeCol>("hud_size", "id", [this](auto* s) { container->hudSizes.push_back(new db_hud_size(s)); });
	load<GraphSettingsCol>("graph_settings", "id",
		[this](auto* s) { setEntity(container->graphSettings, new db_graph_settings(s)); });
	load<HudVarsCol>("hud_size_vars", "id", [this](auto* s) { container->hudVars.push_back(new db_hud_vars(s)); });
	load<ResolutionCol>("resolution", "id", [this](auto* s) { setEntity(container->resolutions, new db_resolution(s)); });
	load<SettingsCol>("settings", nullptr, [this](auto* s) { container->settings = new db_settings(s); });

	sqlite3_close_v2(database);
	database = nullptr;
}

void DatabaseCache::loadData(const std::string& name) {
	if (!openDatabase(name)) {
		exitDatabaseError("Required data database failed to open: " + name);
	}

	load<DbNationCol>("nation", "id DESC", [this](auto* s) { setEntity(container->nations, new db_nation(s)); });
	load<DbUnitCol>("unit", "id DESC", [this](auto* s) { setEntity(container->units, new db_unit(s)); });
	load<DbBuildingCol>("building", "id DESC", [this](auto* s) { setEntity(container->buildings, new db_building(s)); });

	load<DbResourceCol>("resource", "id DESC",
		[this](auto* s) { setEntity(container->resources, new db_resource(s)); });

	load<PlayerColorsCol>("player_color", "id DESC",
		[this](auto* s) { setEntity(container->playerColors, new db_player_colors(s)); });

	load<DbUnitLevelCol>("unit_level", "unit, level", [this](auto* s) {
		auto level = new db_unit_level(s);
		setEntity(container->unitsLevels, level);
		container->units[level->unit]->levels.push_back(level);
	});
	load<DbBuildingLevelCol>("building_level", "building, level", [this](auto* s) {
		auto level = new db_building_level(s);
		setEntity(container->buildingsLevels, level);
		container->buildings[level->building]->levels.push_back(level);
		for (auto nation : container->nations) {
			if (nation) {
				ensureSize(nation->id, level->unitsPerNation);
				ensureSize(nation->id, level->unitsPerNationIds);
				if (level->unitsPerNation[nation->id] == nullptr) {
					level->unitsPerNation[nation->id] = new std::vector<db_unit*>();
				}
				if (level->unitsPerNationIds[nation->id] == nullptr) {
					level->unitsPerNationIds[nation->id] = new std::vector<unsigned short>();
				}
			}
		}
	});

	load<DbUnitNationCol>("unit_to_nation", "unit", [this](auto* s) {
		auto unit = container->units[asUShort(s, DbUnitNationCol::unit)];
		auto nation = container->nations[asUShort(s, DbUnitNationCol::nation)];
		nation->units.push_back(unit);
		if (unit->typeWorker) { nation->workers.push_back(unit); }
		unit->nations.push_back(nation);
	});
	load<DbBuildingNationCol>("building_to_nation", "building", [this](auto* s) {
		auto building = container->buildings[asUShort(s, DbBuildingNationCol::building)];
		auto nation = container->nations[asUShort(s, DbBuildingNationCol::nation)];

		nation->buildings.push_back(building);
		building->nations.push_back(nation);
	});

	load<DbUnitBuildingLevelCol>("unit_to_building_level", "unit", [this](auto* s) {
		auto level = container->buildingsLevels[asUShort(s, DbUnitBuildingLevelCol::building_level)];
		auto unit = container->units[asUShort(s, DbUnitBuildingLevelCol::unit)];
		level->allUnits.push_back(unit);
		for (auto nation : unit->nations) {
			level->unitsPerNation[nation->id]->push_back(unit);
			level->unitsPerNationIds[nation->id]->push_back(unit->id);
		}
	});

	if (!load<DbTechnologyCol>("technology", "id", [this](auto* s) {
		auto* technology = new db_technology(s);
		for (const auto buildingId : technology->researchBuildingIds) {
			if (buildingId >= container->buildings.size() || !container->buildings[buildingId]) {
				exitDatabaseError("Technology " + std::to_string(technology->id) +
					" references missing research building " + std::to_string(buildingId));
			}
		}
		setEntity(container->technologies, technology);
	})) {
		sqlite3_close_v2(database);
		database = nullptr;
		exitDatabaseError("Required data table failed to load: technology");
	}
	if (!load<DbTechnologyLevelCol>("technology_level", "technology, level", [this](auto* s) {
		const auto technologyId = asUShort(s, DbTechnologyLevelCol::technology);
		assert(technologyId < container->technologies.size() && container->technologies[technologyId]);
		auto* level = new db_technology_level(s, container->technologies[technologyId]->name);
		setEntity(container->technologyLevels, level);
		assert(level->technology < container->technologies.size());
		container->technologies[level->technology]->levels.push_back(level);
	})) {
		sqlite3_close_v2(database);
		database = nullptr;
		exitDatabaseError("Required data table failed to load: technology_level");
	}
	if (!load<DbTechnologyEffectCol>("technology_level_effect", "technology_level, id", [this](auto* s) {
		auto* effect = new db_technology_effect(s);
		container->technologyEffects.push_back(effect);
		assert(effect->technologyLevel < container->technologyLevels.size());
		container->technologyLevels[effect->technologyLevel]->effects.push_back(effect);
	})) {
		sqlite3_close_v2(database);
		database = nullptr;
		exitDatabaseError("Required data table failed to load: technology_level_effect");
	}

	container->finish();

	sqlite3_close_v2(database);
	database = nullptr;
}

void DatabaseCache::loadMaps(const std::string& name) {
	if (!openDatabase(name)) { exitDatabaseError("Required data database failed to open: " + name); }

	load<DbWorldAgeCol>("age", "id DESC", [this](auto* s) {
		setEntity(container->worldAgeCatalog.ages, new db_world_age(s));
	});
	load<DbAgeConditionCol>("age_condition", "age_id, metric, target", [this](auto* s) {
		auto* condition = new db_age_condition(s);
		container->worldAgeCatalog.ageConditions.push_back(condition);
		auto* age = container->worldAgeCatalog.getAge(condition->ageId);
		assert(age != nullptr && "age_condition must reference an existing age");
		age->ageConditions.push_back(condition);
	});
	load<MapCol>("map", "id DESC", [this](auto* s) { setEntity(container->maps, new db_map(s)); });
	load<DbMissionGoalCol>("mission_goal", "id", [this](auto* s) {
		setEntity(container->missionGoals, new db_mission_goal(s));
	});
	for (size_t i = 0; i < container->missionGoals.size(); ++i) {
		assert(container->missionGoals[i] && container->missionGoals[i]->id == i &&
			"mission goal IDs must be dense and match their database IDs");
	}
	for (auto* map : container->maps) {
		if (!map) continue;
		for (size_t i = 0; i < map->goalIds.size(); ++i) {
			assert(std::find(map->goalIds.begin(), map->goalIds.begin() + i, map->goalIds[i]) == map->goalIds.begin() + i &&
				"map goal_ids must not contain duplicate IDs");
		}
		for (const auto goalId : map->goalIds) {
			if (goalId >= container->missionGoals.size() || !container->missionGoals[goalId]) {
				exitDatabaseError("Map " + std::to_string(map->id) + " references missing mission goal " +
					std::to_string(goalId));
			}
			map->goals.push_back(container->missionGoals[goalId]);
		}
		for (const auto* goal : map->goals) {
			assert(goal->targetValue >= 0.f && "mission goal targets must be non-negative");
			assert(goal->targetPlayer >= -1 && "mission goal target player must be -1 or a player ID");
			if (goal->type == MissionGoalType::ELIMINATE_PLAYER || goal->type == MissionGoalType::ELIMINATE_TEAM) {
				assert(goal->targetPlayer >= 0 && "targeted elimination goals require target_player");
			}
			if (goal->type == MissionGoalType::RESOURCE_STOCK || goal->type == MissionGoalType::RESOURCE_GATHERED) {
				assert(goal->targetId >= 0 && goal->targetId < 4 && "resource goals require a resource ID from 0 to 3");
			}
			if (goal->type == MissionGoalType::BUILDING_COUNT || goal->type == MissionGoalType::SURVIVE_WITH_BUILDING_UNTIL) {
				assert(goal->targetId >= -1 && (goal->targetId < 0 ||
					(goal->targetId < container->buildings.size() && container->buildings[goal->targetId])));
			}
			assert(goal->timeTicks > 0 || (goal->type != MissionGoalType::SURVIVE_UNTIL &&
				goal->type != MissionGoalType::SURVIVE_WITH_BUILDING_UNTIL &&
				goal->type != MissionGoalType::SCORE_LEAD_AT_TIME));
		}
	}
	validateWorldAgeCatalog();

	sqlite3_close_v2(database);
	database = nullptr;
}

void DatabaseCache::validateWorldAgeCatalog() const {
	const auto& catalog = container->worldAgeCatalog;
	assert(!catalog.ages.empty() && catalog.getAge(0) != nullptr && "age catalog must define age 0");
	assert(catalog.getAge(0)->stage == 0 && "age 0 must be the starting age");
	for (const auto* age : catalog.ages) {
		if (!age) {
			continue;
		}
		assert(!age->name.Empty());
		assert(age->stage == 0 || !age->ageConditions.empty());
		for (const auto* condition : age->ageConditions) {
			assert(condition != nullptr);
			assert(static_cast<unsigned char>(condition->metric) <= static_cast<unsigned char>(WorldAgeMetric::ARMY_COUNT));
			assert(condition->target > 0.f);
		}
	}
	for (const auto* map : container->maps) {
		if (!map) {
			continue;
		}
		assert(!map->ageIds.empty() && "map must define at least one age");
		assert(map->ageIds.front() == 0 && "map age list must start at age 0");
		std::vector<bool> seen(catalog.ages.size(), false);
		unsigned char lastStage = 0;
		for (const auto ageId : map->ageIds) {
			assert(ageId < seen.size() && catalog.getAge(ageId) != nullptr);
			assert(!seen[ageId] && "map age list must not contain duplicate IDs");
			seen[ageId] = true;
			const auto stage = catalog.getAge(ageId)->stage;
			assert(stage >= lastStage && stage <= lastStage + 1 && "map age stages must be ordered without gaps");
			lastStage = stage;
		}
	}
	for (const auto* level : container->unitsLevels) {
		assert(level != nullptr && level->ageStage <= catalog.maxStage());
	}
	for (const auto* level : container->buildingsLevels) {
		assert(level != nullptr && level->ageStage <= catalog.maxStage());
	}
}


DatabaseCache::~DatabaseCache() {
	if (database) { sqlite3_close_v2(database); }
	delete container;
}

void DatabaseCache::setGraphSettings(int id, db_graph_settings* gs) {
	gs->name = container->graphSettings[id]->name;
	gs->styles = container->graphSettings[id]->styles;
	delete container->graphSettings[id];
	container->graphSettings[id] = gs;
	if (!openDatabase("base.db", false)) { return; }
	DbUpdate stmt(database,
				"UPDATE graphics_settings SET "
				"hud_size = :hud_size, "
	              "fullscreen = :fullscreen, "
	              "max_fps = :max_fps, "
	              "min_fps = :min_fps, "
	              "name = :name, "
	              "v_sync = :v_sync, "
	              "shadow = :shadow, "
	              "texture_quality = :texture_quality "
	              "WHERE id = :id;");

	stmt.bind(":hud_size", gs->hud_size)
	    .bind(":fullscreen", gs->fullscreen)
	    .bind(":max_fps", gs->max_fps)
	    .bind(":min_fps", gs->min_fps)
	    .bind(":name", gs->name)
	    .bind(":v_sync", gs->v_sync)
	    .bind(":shadow", gs->shadow)
	    .bind(":texture_quality", gs->texture_quality)
	    .bind(":id", id)
	    .execAndClose();
	sqlite3_close_v2(database);
}

void DatabaseCache::setSettings(db_settings* settings) {
	settings->graph = 0;
	delete container->settings;
	container->settings = settings;
	if (!openDatabase("base.db", false)) { return; }
	DbUpdate stmt(database,
	              "UPDATE settings SET "
	              "graph = :graph, "
	              "resolution = :resolution;");

	stmt.bind(":graph", settings->graph)
	    .bind(":resolution", settings->resolution)
	    .execAndClose();
	sqlite3_close_v2(database);
}

void DatabaseCache::refreshAfterParametersRead() const {
	for (const auto nation : container->nations) { nation->refresh(); }
}
