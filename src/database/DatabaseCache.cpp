#include "DatabaseCache.h"
#include <sqlite3/sqlite3.h>
#include "db_strcut.h"
#include <iostream>
#include <sstream>
#include "db_units.h"

static unsigned fromHex(char** argv, int index) {
	unsigned x;
	std::stringstream ss;
	ss << std::hex << argv[index];
	ss >> x;
	return x;
}

int static loadUnits(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->units[id] = new db_unit(id, argv[1], atof(argv[2]), atof(argv[3]), argv[5], argv[6], argv[7],
	                             atof(argv[8]), atof(argv[9]), atof(argv[10]), atoi(argv[11]), atoi(argv[12]), argv[13],
	                             atoi(argv[14]));
	xyz->units_size++;
	return 0;
}

int static loadHudSizes(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->hudSizes[id] = new db_hud_size(id, argv[1]);
	xyz->hud_size_size++;
	return 0;
}

int static loadGraphSettings(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->graphSettings[id] = new db_graph_settings(id, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5]),
	                                               atof(argv[6]), atof(argv[7]));
	xyz->graph_settings_size++;
	return 0;
}

int static loadBuildings(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->buildings[id] = new db_building(id, argv[1], atoi(argv[2]), atoi(argv[3]), argv[5], argv[6],
	                                     argv[7], atof(argv[8]), argv[9], atoi(argv[10]), argv[11], atoi(argv[12]));
	xyz->building_size++;
	return 0;
}

int static loadNation(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->nations[id] = new db_nation(id, argv[1]);
	xyz->nation_size++;
	return 0;
}

int static loadResource(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->resources[id] = new db_resource(id, argv[1], argv[2], atoi(argv[3]), argv[4], argv[5], atof(argv[6]),
	                                     atoi(argv[7]), atoi(argv[8]), atoi(argv[9]), fromHex(argv, 10));
	xyz->resource_size++;

	return 0;
}

int static loadHudVars(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->hudVars[id] = new db_hud_vars(id, atoi(argv[1]), argv[2], atof(argv[3]));
	xyz->hud_vars_size++;

	return 0;
}

int static loadBuildingToUnit(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int buildingId = atoi(argv[1]);
	xyz->unitsForBuilding[buildingId]->push_back(xyz->units[atoi(argv[2])]);

	return 0;
}

int static loadCostUnit(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int unitId = atoi(argv[3]);
	const int resourceId = atoi(argv[1]);
	db_resource* dbResource = xyz->resources[resourceId];
	xyz->costForUnit[unitId]->push_back(new db_cost(atoi(argv[0]), resourceId, atoi(argv[2]), dbResource->name, unitId));

	return 0;
}

int static loadCostBuilding(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int buildingId = atoi(argv[3]);
	const int resourceId = atoi(argv[1]);
	db_resource* dbResource = xyz->resources[resourceId];
	xyz->costForBuilding[buildingId]->push_back(new db_cost(atoi(argv[0]), resourceId, atoi(argv[2]), dbResource->name,
	                                                        buildingId));

	return 0;
}

int static loadOrders(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->orders[id] = new db_order(id, argv[1]);
	xyz->orders_size++;
	return 0;
}

int static loadOrdersToUnit(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	const int orderId = atoi(argv[2]);
	const int unitId = atoi(argv[1]);
	db_order* dbOrder = xyz->orders[orderId];
	xyz->ordersToUnit[unitId]->push_back(dbOrder);

	return 0;
}

int static loadMap(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);

	xyz->maps[id] = new db_map(id, argv[1], argv[2], atof(argv[3]), atof(argv[4]));
	xyz->maps_size++;
	return 0;
}

int static loadPlayerColors(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = static_cast<db_container *>(data);
	const int id = atoi(argv[0]);
	xyz->playerColors[id] = new db_player_colors(id, fromHex(argv, 1), fromHex(argv, 2));
	xyz->player_colors_size++;
	return 0;
}

void DatabaseCache::execute(const char* sql, int (*load)(void*, int, char**, char**)) {
	char* error;
	const int rc = sqlite3_exec(database, sql, load, dbContainer, &error);
	ifError(rc, error);
}

DatabaseCache::DatabaseCache() {
	const int rc = sqlite3_open("Data/Database/base.db", &database);
	if (rc) {
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << endl << endl;
		sqlite3_close(database);
		return;
	}

	dbContainer = new db_container();

	execute("SELECT * from units", loadUnits);
	execute("SELECT * from hud_size", loadHudSizes);
	execute("SELECT * from graph_settings", loadGraphSettings);
	execute("SELECT * from building", loadBuildings);
	execute("SELECT * from nation", loadNation);
	execute("SELECT * from resource", loadResource);
	execute("SELECT * from hud_size_vars", loadHudVars);
	execute("SELECT * from building_to_unit", loadBuildingToUnit);
	execute("SELECT * from cost_building", loadCostBuilding);
	execute("SELECT * from cost_unit", loadCostUnit);
	execute("SELECT * from orders", loadOrders);
	execute("SELECT * from orders_to_unit", loadOrdersToUnit);
	execute("SELECT * from map", loadMap);
	execute("SELECT * from player_colors", loadPlayerColors);


	sqlite3_close(database);
}


DatabaseCache::~DatabaseCache() {
	delete dbContainer;
}

db_unit* DatabaseCache::getUnit(int i) {
	return dbContainer->units[i];
}

db_hud_size* DatabaseCache::getHudSize(int i) {
	return dbContainer->hudSizes[i];
}

db_graph_settings* DatabaseCache::getGraphSettings(int i) {
	return dbContainer->graphSettings[i];
}

db_building* DatabaseCache::getBuilding(int i) {
	return dbContainer->buildings[i];
}

db_nation* DatabaseCache::getNation(int i) {
	return dbContainer->nations[i];
}

db_resource* DatabaseCache::getResource(int i) {
	return dbContainer->resources[i];
}

db_hud_vars* DatabaseCache::getHudVar(int i) {
	return dbContainer->hudVars[i];
}

db_order* DatabaseCache::getOrder(int i) {
	return dbContainer->orders[i];
}

db_map* DatabaseCache::getMap(int i) {
	return dbContainer->maps[i];
}

db_player_colors* DatabaseCache::getPlayerColor(int i) {
	return dbContainer->playerColors[i];
}

int DatabaseCache::getResourceSize() {
	return dbContainer->resource_size;
}

int DatabaseCache::getHudVarsSize() {
	return dbContainer->hud_vars_size;
}

int DatabaseCache::getBuildingSize() {
	return dbContainer->building_size;
}

int DatabaseCache::getUnitSize() {
	return dbContainer->units_size;
}

int DatabaseCache::getOrdersSize() {
	return dbContainer->orders_size;
}

int DatabaseCache::getMapSize() {
	return dbContainer->maps_size;
}

int DatabaseCache::getPlayerColorsSize() {
	return dbContainer->player_colors_size;
}

std::vector<db_unit*>* DatabaseCache::getUnitsForBuilding(int id) {
	return dbContainer->unitsForBuilding[id];
}

std::vector<db_cost*>* DatabaseCache::getCostForUnit(int id) {
	return dbContainer->costForUnit[id];
}

std::vector<db_cost*>* DatabaseCache::getCostForBuilding(int id) {
	return dbContainer->costForBuilding[id];
}

std::vector<db_order*>* DatabaseCache::getOrdersForUnit(int id) {
	return dbContainer->ordersToUnit[id];
}
