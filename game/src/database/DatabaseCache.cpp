#include "DatabaseCache.h"

#include <iostream>

#include "db_grah_structs.h"
#include "db_load.h"
#include "db_utils.h"
#include "scene/save/SQLConsts.h"

void DatabaseCache::execute(const std::string& sql, int (*load)(void*, int, char**, char**)) const {
	char* error;
	const int rc = sqlite3_exec(database, sql.c_str(), load, container, &error);
	ifError(rc, error, sql);
}

bool DatabaseCache::openDatabase(const std::string& name) {
	const int rc = sqlite3_open_v2((pathStr + name).c_str(), &database, SQLITE_OPEN_READONLY, nullptr);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << " " << name << std::endl;
		sqlite3_close_v2(database);
		return true;
	}
	//std::cout << "Database Opened: " << name << std::endl;
	return false;
}

DatabaseCache::DatabaseCache(std::string postfix) {
	container = new db_container();

	pathStr = std::string("Data/");
	if (!SIM_GLOBALS.HEADLESS) {
		loadBasic("Database/base" + postfix);
	}

	loadData("Database/data" + postfix);
	loadMaps("map/maps" + postfix);
}

void DatabaseCache::loadBasic(const std::string& name) {
	if (openDatabase(name)) { return; }

	load("hud_size", [this](auto* s){ container->hudSizes.push_back(new db_hud_size(s)); });
	load("graph_settings", [this](auto* s){ setEntity(container->graphSettings, new db_graph_settings(s)); });
	load("hud_size_vars", [this](auto* s){ container->hudVars.push_back(new db_hud_vars(s)); });
	load("resolution", [this](auto* s){ setEntity(container->resolutions, new db_resolution(s)); });
	load("settings", [this](auto* s){ container->settings = new db_settings(s); });

	sqlite3_close_v2(database);
}

void DatabaseCache::loadData(const std::string& name) {
	if (openDatabase(name)) { return; }

	load("nation order by id desc", [this](auto* s){ setEntity(container->nations, new db_nation(s)); });
	load("unit order by id desc", [this](auto* s){ setEntity(container->units, new db_unit(s)); });
	load("building order by id desc", [this](auto* s){ setEntity(container->buildings, new db_building(s)); });

	load("resource order by id desc", [this](auto* s){ setEntity(container->resources, new db_resource(s)); });

	load("player_color order by id desc", [this](auto* s){
		setEntity(container->playerColors, new db_player_colors(s));
	});

	load("unit_level order by unit,level", [this](auto* s){
		auto level = new db_unit_level(s);
		setEntity(container->unitsLevels, level);
		container->units[level->unit]->levels.push_back(level);
	});
	load("building_level order by level", [this](auto* s){
		auto level = new db_building_level(s);
		setEntity(container->buildingsLevels, level);
		container->buildings[level->building]->levels.push_back(level);
		for (auto nation : container->nations) {
			if (nation) {
				ensureSize(nation->id + 1, level->unitsPerNation);
				ensureSize(nation->id + 1, level->unitsPerNationIds);
				if (level->unitsPerNation[nation->id] == nullptr) {
					level->unitsPerNation[nation->id] = new std::vector<db_unit*>();
				}
				if (level->unitsPerNationIds[nation->id] == nullptr) {
					level->unitsPerNationIds[nation->id] = new std::vector<unsigned char>();
				}
			}
		}
	});

	load("unit_to_nation order by unit", [this](auto* s){
		auto unit = container->units[atoi(argv[0])];
		auto nation = container->nations[atoi(argv[1])];
		nation->units.push_back(unit);
		if (unit->typeWorker) {
			nation->workers.push_back(unit);
		}
		unit->nations.push_back(nation);
	});
	load("building_to_nation order by building", [this](auto* s){
		auto building = container->buildings[atoi(argv[0])];
		auto nation = container->nations[atoi(argv[1])];

		nation->buildings.push_back(building);
		building->nations.push_back(nation);
	});

	load("unit_to_building_level order by unit", [this](auto* s){
		auto level = container->buildingsLevels[atoi(argv[0])];
		auto unit = container->units[atoi(argv[1])];
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
	if (openDatabase(name)) { return; }

	load("map order by id desc", [this](auto* s) { setEntity(container->maps, new db_map(s)); });

	sqlite3_close_v2(database);
}


DatabaseCache::~DatabaseCache() {
	delete container;
}

void DatabaseCache::executeSingleBasic(const std::string& name, const char* sql) {
	if (openDatabase(name)) { return; }
	execute(sql, callback);
	sqlite3_close_v2(database);
}

void DatabaseCache::setGraphSettings(int i, db_graph_settings* graphSettings) {
	graphSettings->name = container->graphSettings[i]->name;
	graphSettings->styles = container->graphSettings[i]->styles;
	delete container->graphSettings[i];
	container->graphSettings[i] = graphSettings;
	Urho3D::String sql = "UPDATE graph_settings";
	sql.Append(" SET hud_size = ").Append(Urho3D::String(graphSettings->hud_size))
	   //.Append("SET style =").Append(Urho3D::String(graphSettings->hud_size));
	   .Append(", fullscreen = ").Append(Urho3D::String((int)graphSettings->fullscreen))
	   .Append(", max_fps =").Append(Urho3D::String(graphSettings->max_fps))
	   .Append(", min_fps =").Append(Urho3D::String(graphSettings->min_fps))
	   .Append(", name = ").Append("'" + Urho3D::String(graphSettings->name) + "'")
	   .Append(", v_sync = ").Append(Urho3D::String((int)graphSettings->v_sync))
	   .Append(", shadow = ").Append(Urho3D::String((int)graphSettings->shadow))
	   .Append(", texture_quality =").Append(Urho3D::String(graphSettings->texture_quality))
	   .Append(" WHERE id =").Append(Urho3D::String(i));

	executeSingleBasic("base.db", sql.CString());
}

void DatabaseCache::setSettings(int i, db_settings* settings) {
	settings->graph = 0;
	delete container->settings;
	container->settings = settings;
	Urho3D::String sql = "UPDATE settings";
	sql.Append(" SET graph = ").Append(Urho3D::String(settings->graph))
	   .Append(", resolution = ").Append(Urho3D::String(settings->resolution));

	executeSingleBasic("base.db", sql.CString());
}

void DatabaseCache::refreshAfterParametersRead() const {
	for (const auto nation : container->nations) {
		nation->refresh();
	}
}

//TODO zrobiæ listê str -> [this](sqlite3_stmt* stmt) i wczytac?
void DatabaseCache::loadNation() {}

template <typename Creator>
void DatabaseCache::load(const std::string& tableName, Creator createFn) {
	std::string sqlStr = SQLConsts::SELECT + tableName;
	const char* sql = sqlStr.c_str();
	sqlite3_stmt* stmt = nullptr;

	if (sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr) != SQLITE_OK) {
		// obs³uga b³êdu
		return;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		createFn(stmt);
	}

	sqlite3_finalize(stmt);
}
