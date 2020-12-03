#include "DatabaseCache.h"

#include <iostream>

#include "db_grah_structs.h"
#include "db_load.h"
#include "db_utils.h"

void DatabaseCache::execute(const char* sql, int (*load)(void*, int, char**, char**)) const {
	char* error;
	const int rc = sqlite3_exec(database, sql, load, dbContainer, &error);
	ifError(rc, error);
}

bool DatabaseCache::openDatabase(const std::string& name) {
	const int rc = sqlite3_open((pathStr + name).c_str(), &database);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close(database);
		return true;
	}
	std::cout << "Database Opened: " << name << std::endl;
	return false;
}

DatabaseCache::DatabaseCache(const char* path) {
	dbContainer = new db_container();

	pathStr = std::string(path);
	pathStr.append("/Data/");

	loadBasic("Database/base.db");
	loadData("Database/data.db");
	loadMaps("map/maps.db");
}

void DatabaseCache::loadBasic(const std::string& name) {
	if (openDatabase(name)) { return; }

	execute("SELECT * from hud_size", loadHudSizes);
	execute("SELECT * from graph_settings", loadGraphSettings);
	execute("SELECT * from hud_size_vars", loadHudVars);
	execute("SELECT * from resolution", loadResolution);
	execute("SELECT * from settings", loadSettings);

	sqlite3_close(database);
}

void DatabaseCache::loadData(const std::string& name) {
	if (openDatabase(name)) { return; }

	execute("SELECT * from nation order by id desc", loadNation);
	execute("SELECT * from unit order by id desc", loadUnits);
	execute("SELECT * from building order by id desc", loadBuildings);

	execute("SELECT * from resource order by id desc", loadResource);
	execute("SELECT * from building_cost", loadCostBuilding);
	execute("SELECT * from unit_cost", loadCostUnit);

	execute("SELECT * from orders order by id desc", loadOrders);

	execute("SELECT * from player_color order by id desc", loadPlayerColors);

	execute("SELECT * from unit_level order by level", loadUnitLevels);
	execute("SELECT * from building_level order by level", loadBuildingLevels);

	execute("SELECT * from building_to_nation order by building", loadBuildingToNation);
	execute("SELECT * from unit_to_nation order by unit", loadUnitToNation);

	execute("SELECT * from unit_to_building_level order by unit", loadUnitToBuildingLevels);//TODO make sure its sorted set_intersection

	execute("SELECT * from unit_level_cost", loadCostUnitLevel);
	execute("SELECT * from building_level_cost", loadCostBuildingLevel);

	dbContainer->finish();

	sqlite3_close(database);
}

void DatabaseCache::loadMaps(const std::string& name) {
	if (openDatabase(name)) { return; }

	execute("SELECT * from map order by id desc", loadMap);
	
	sqlite3_close(database);
}


DatabaseCache::~DatabaseCache() {
	delete dbContainer;
}

void DatabaseCache::executeSingleBasic(const std::string& name, const char* sql) {
	if (openDatabase(name)) { return; }
	execute(sql, callback);
	sqlite3_close(database);
}

void DatabaseCache::setGraphSettings(int i, db_graph_settings* graphSettings) {
	graphSettings->name = dbContainer->graphSettings[i]->name;
	graphSettings->styles = dbContainer->graphSettings[i]->styles;
	delete dbContainer->graphSettings[i];
	dbContainer->graphSettings[i] = graphSettings;
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
	delete dbContainer->settings[0];
	dbContainer->settings[0] = settings;
	Urho3D::String sql = "UPDATE settings";
	sql.Append(" SET graph = ").Append(Urho3D::String(settings->graph))
	   .Append(", resolution = ").Append(Urho3D::String(settings->resolution));

	executeSingleBasic("base.db", sql.CString());
}
