#include "DatabaseCache.h"

#include "db_grah_structs.h"
#include "db_update_utils.h"
#include "db_utils.h"

bool DatabaseCache::openDatabase(const std::string& name) {
	database = openDb(pathStr + name);
	return database;
}

DatabaseCache::DatabaseCache() {
	container = new db_container();

	pathStr = std::string("Data/");
	if (!SIM_GLOBALS.HEADLESS) { loadBasic("Database/base.db"); }

	loadData("Database/data.db");
	loadMaps("map/maps.db");
}

void DatabaseCache::loadBasic(const std::string& name) {
	if (!openDatabase(name)) { return; }

	load("hud_size", [this](auto* s) { container->hudSizes.push_back(new db_hud_size(s)); });
	load("graph_settings", [this](auto* s) { setEntity(container->graphSettings, new db_graph_settings(s)); });
	load("hud_size_vars", [this](auto* s) { container->hudVars.push_back(new db_hud_vars(s)); });
	load("resolution", [this](auto* s) { setEntity(container->resolutions, new db_resolution(s)); });
	load("settings", [this](auto* s) { container->settings = new db_settings(s); });

	sqlite3_close_v2(database);
}

void DatabaseCache::loadData(const std::string& name) {
	if (!openDatabase(name)) { return; }

	load("nation order by id desc", [this](auto* s) { setEntity(container->nations, new db_nation(s)); });
	load("unit order by id desc", [this](auto* s) { setEntity(container->units, new db_unit(s)); });
	load("building order by id desc", [this](auto* s) { setEntity(container->buildings, new db_building(s)); });

	load("resource order by id desc",
		[this](auto* s) { setEntity(container->resources, new db_resource(s)); });

	load("player_color order by id desc",
		[this](auto* s) { setEntity(container->playerColors, new db_player_colors(s)); });

	load("unit_level order by unit,level", [this](auto* s) {
		auto level = new db_unit_level(s);
		setEntity(container->unitsLevels, level);
		container->units[level->unit]->levels.push_back(level);
	});
	load("building_level order by level", [this](auto* s) {
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
					level->unitsPerNationIds[nation->id] = new std::vector<unsigned char>();
				}
			}
		}
	});

	load("unit_to_nation order by unit", [this](auto* s) {
		auto unit = container->units[asShort(s, 0)];
		auto nation = container->nations[asShort(s, 1)];
		nation->units.push_back(unit);
		if (unit->typeWorker) { nation->workers.push_back(unit); }
		unit->nations.push_back(nation);
	});
	load("building_to_nation order by building", [this](auto* s) {
		auto building = container->buildings[asShort(s, 0)];
		auto nation = container->nations[asShort(s, 1)];

		nation->buildings.push_back(building);
		building->nations.push_back(nation);
	});

	load("unit_to_building_level order by unit", [this](auto* s) {
		auto level = container->buildingsLevels[asShort(s, 0)];
		auto unit = container->units[asShort(s, 1)];
		level->allUnits.push_back(unit);
		for (auto nation : unit->nations) {
			level->unitsPerNation[nation->id]->push_back(unit);
			level->unitsPerNationIds[nation->id]->push_back(unit->id);
		}
	});
	//TODO make sure its sorted set_intersection

	container->finish();

	sqlite3_close_v2(database);
}

void DatabaseCache::loadMaps(const std::string& name) {
	if (!openDatabase(name)) { return; }

	load("map order by id desc", [this](auto* s) { setEntity(container->maps, new db_map(s)); });

	sqlite3_close_v2(database);
}


DatabaseCache::~DatabaseCache() { delete container; }

void DatabaseCache::setGraphSettings(int id, db_graph_settings* gs) {
	gs->name = container->graphSettings[id]->name;
	gs->styles = container->graphSettings[id]->styles;
	delete container->graphSettings[id];
	container->graphSettings[id] = gs;
	if (!openDatabase("base.db")) { return; }
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
	sqlite3_close(database);
}

void DatabaseCache::setSettings(db_settings* settings) {
	settings->graph = 0;
	delete container->settings;
	container->settings = settings;
	if (!openDatabase("base.db")) { return; }
	DbUpdate stmt(database,
	              "UPDATE settings SET "
	              "graph = :graph, "
	              "resolution = :resolution;");

	stmt.bind(":graph", settings->graph)
	    .bind(":resolution", settings->resolution)
	    .execAndClose();
	sqlite3_close(database);
}

void DatabaseCache::refreshAfterParametersRead() const {
	for (const auto nation : container->nations) { nation->refresh(); }
}
