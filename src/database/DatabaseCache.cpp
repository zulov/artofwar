#include "DatabaseCache.h"
#include "db_strcut.h"
#include "db_utils.h"
#include <iostream>
#include <sqlite3/sqlite3.h>
#include <sstream>
#include "Game.h"

static unsigned fromHex(char** argv, int index) {
	unsigned x;
	std::stringstream ss;
	ss << std::hex << argv[index];
	ss >> x;
	return x;
}

int static loadUnits(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	ensureSize(id, xyz->units);

	xyz->units[id] = new db_unit(id, argv[1], atoi(argv[2]), argv[4], atoi(argv[4]));

	return 0;
}

int static loadHudSizes(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	xyz->hudSizes.push_back(new db_hud_size(id, argv[1]));

	return 0;
}

int static loadGraphSettings(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	ensureSize(id, xyz->graphSettings);

	xyz->graphSettings[id] = new db_graph_settings(id, atoi(argv[1]), argv[2], atoi(argv[3]),
	                                               atof(argv[4]), atof(argv[5]), argv[6],
	                                               atoi(argv[7]), atoi(argv[8]), atoi(argv[9]));
	return 0;
}

int static loadBuildings(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);

	ensureSize(id, xyz->buildings);
	xyz->buildings[id] = new db_building(id, argv[1], atoi(argv[2]), atoi(argv[3]), argv[4]);

	return 0;
}

int static loadNation(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	ensureSize(id, xyz->nations);

	xyz->nations[id] = new db_nation(id, argv[1]);

	return 0;
}

int static loadResource(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	ensureSize(id, xyz->resources);

	xyz->resources[id] = new db_resource(id, argv[1], argv[2], atoi(argv[3]), argv[4],
	                                     atoi(argv[5]), atoi(argv[6]), atoi(argv[7]), fromHex(argv, 8));

	return 0;
}

int static loadHudVars(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	xyz->hudVars.push_back(new db_hud_vars(id, atoi(argv[1]), argv[2], atof(argv[3])));

	return 0;
}

static void addCost(char** argv, db_container* const xyz, std::vector<db_cost*>& array) {
	array.push_back(new db_cost(atoi(argv[1]), atoi(argv[2])));
}

int static loadCostUnit(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	addCost(argv, xyz, xyz->units[atoi(argv[0])]->costs);

	return 0;
}

int static loadCostUnitLevel(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	addCost(argv, xyz, xyz->unitsLevels[atoi(argv[0])]->costs);

	return 0;
}

int static loadCostBuildingLevel(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	addCost(argv, xyz, xyz->buildingsLevels[atoi(argv[0])]->costs);
	return 0;
}

int static loadCostBuilding(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);

	addCost(argv, xyz, xyz->buildings[atoi(argv[0])]->costs);
	return 0;
}

int static loadOrders(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	ensureSize(id, xyz->orders);

	xyz->orders[id] = new db_order(id, argv[1], argv[2]);

	return 0;
}

int static loadOrdersToUnit(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	db_order* dbOrder = xyz->orders[atoi(argv[1])];

	xyz->units[atoi(argv[0])]->orders.push_back(dbOrder);

	return 0;
}

int static loadMap(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	ensureSize(id, xyz->maps);

	xyz->maps[id] = new db_map(id, argv[1], argv[2], atof(argv[3]), atof(argv[4]), argv[5]);

	return 0;
}

int static loadPlayerColors(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	ensureSize(id, xyz->playerColors);
	xyz->playerColors[id] = new db_player_colors(id, fromHex(argv, 1), fromHex(argv, 2), argv[3]);

	return 0;
}

int static loadResolution(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	const int id = atoi(argv[0]);
	ensureSize(id, xyz->resolutions);

	xyz->resolutions[id] = new db_resolution(id, atoi(argv[1]), atoi(argv[2]));
	return 0;
}

int static loadSettings(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	ensureSize(1, xyz->settings);
	xyz->settings[0] = new db_settings(atoi(argv[0]), atoi(argv[1]));

	return 0;
}


int static loadBuildingLevels(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	auto levelId = atoi(argv[0]);
	ensureSize(levelId, xyz->buildingsLevels);
	xyz->buildingsLevels[levelId] = new db_building_level(levelId, atoi(argv[1]), atoi(argv[2]), argv[3],
	                                                      argv[4], atoi(argv[5]));
	xyz->buildings[atoi(argv[2])]->levels.push_back(xyz->buildingsLevels[levelId]);
	return 0;
}

int static loadUnitLevels(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	auto levelId = atoi(argv[0]);
	int unitId = atoi(argv[2]);

	ensureSize(levelId, xyz->unitsLevels);
	xyz->unitsLevels[levelId] = new db_unit_level(levelId, atoi(argv[1]), atoi(argv[2]), argv[3], atof(argv[4]),
	                                              atof(argv[5]), argv[6], atof(argv[7]), atof(argv[8]),
	                                              atoi(argv[9]), atof(argv[10]), atof(argv[11]), atoi(argv[12]),
	                                              atof(argv[13]), atof(argv[14]), atof(argv[15]),
	                                              atof(argv[16]), atof(argv[17]));

	xyz->units[unitId]->levels.push_back(xyz->unitsLevels[levelId]);

	return 0;
}

int static loadUnitToNation(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	int unitId = atoi(argv[0]);
	int nationId = atoi(argv[1]);
	xyz->nations[nationId]->units.push_back(xyz->units[unitId]);
	xyz->units[unitId]->nations.push_back(xyz->nations[nationId]);
	return 0;
}

int static loadBuildingToNation(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	int buildingId = atoi(argv[0]);
	int nationId = atoi(argv[1]);
	xyz->nations[nationId]->buildings.push_back(xyz->buildings[buildingId]);
	xyz->buildings[buildingId]->nations.push_back(xyz->nations[nationId]);
	return 0;
}

int static loadUnitToBuildingLevels(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<db_container*>(data);
	int unitId = atoi(argv[0]);
	int buildingLevelId = atoi(argv[1]);
	auto level = xyz->buildingsLevels[buildingLevelId];

	level->allUnits.push_back(xyz->units[unitId]);

	//level->unitsPerNation.resize(xyz)
	return 0;
}

static int callback(void* data, int argc, char** argv, char** azColName) {
	return 0;
}

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
	pathStr.append("/Data/Database/");

	loadBasic("base.db");
	loadData("data.db");
}

void DatabaseCache::loadBasic(std::string name) {
	if (openDatabase(name)) { return; }

	execute("SELECT * from hud_size", loadHudSizes);
	execute("SELECT * from graph_settings", loadGraphSettings);
	execute("SELECT * from hud_size_vars", loadHudVars);
	execute("SELECT * from resolution", loadResolution);
	execute("SELECT * from settings", loadSettings);

	sqlite3_close(database);
}

void DatabaseCache::loadData(std::string name) {
	if (openDatabase(name)) { return; }

	execute("SELECT * from map", loadMap);

	execute("SELECT * from nation order by id desc", loadNation);
	execute("SELECT * from unit order by id desc", loadUnits);
	execute("SELECT * from building order by id desc", loadBuildings);

	execute("SELECT * from resource order by id desc", loadResource);
	execute("SELECT * from building_cost", loadCostBuilding);
	execute("SELECT * from unit_cost", loadCostUnit);

	execute("SELECT * from orders order by id desc", loadOrders);
	execute("SELECT * from order_to_unit", loadOrdersToUnit);

	execute("SELECT * from player_color order by id desc", loadPlayerColors);

	execute("SELECT * from unit_level order by level", loadUnitLevels);
	execute("SELECT * from building_level order by level", loadBuildingLevels);

	execute("SELECT * from building_to_nation", loadBuildingToNation);
	execute("SELECT * from unit_to_nation", loadUnitToNation);

	execute("SELECT * from unit_to_building_level", loadUnitToBuildingLevels);

	execute("SELECT * from unit_level_cost", loadCostUnitLevel);
	execute("SELECT * from building_level_cost", loadCostBuildingLevel);

	//execute("SELECT * from ai_prop_level order by level,building", loadCostBuildingLevel);

	sqlite3_close(database);
}


DatabaseCache::~DatabaseCache() {
	delete dbContainer;
}

void DatabaseCache::executeSingleBasic(std::string name, const char* sql) {
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
