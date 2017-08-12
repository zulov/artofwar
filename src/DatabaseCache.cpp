#include "DatabaseCache.h"
#include <sqlite3/sqlite3.h>
#include "db_strcut.h"

int static loadUnits(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int id = atoi(argv[0]);
	xyz->units[id] = new db_unit(id, argv[1], atof(argv[2]), atof(argv[3]), atoi(argv[4]), argv[5], argv[6], argv[7], atof(argv[8]), atof(argv[9]), atof(argv[10]), atoi(argv[11]), atoi(argv[12]), argv[13]);
	xyz->units_size++;
	return 0;
}

int static loadHudSizes(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int id = atoi(argv[0]);
	xyz->hudSizes[id] = new db_hud_size(id, argv[1]);
	xyz->hud_size_size++;
	return 0;
}

int static loadGraphSettings(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int id = atoi(argv[0]);
	xyz->graphSettings[id] = new db_graph_settings(id, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5]), atof(argv[6]), atof(argv[7]));
	xyz->graph_settings_size++;
	return 0;
}

int static loadBuildings(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int id = atoi(argv[0]);
	xyz->buildings[id] = new db_building(id, argv[1], atof(argv[2]), atoi(argv[3]), argv[4], argv[5], argv[6], atof(argv[7]), argv[8], atoi(argv[9]), argv[10]);
	xyz->building_size++;
	return 0;
}

int static loadBuildingType(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int id = atoi(argv[0]);
	xyz->buildingTypes[id] = new db_building_type(id, argv[1]);
	xyz->building_type_size++;
	return 0;
}

int static loadUnitType(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int id = atoi(argv[0]);
	xyz->unitTypes[id] = new db_unit_type(id, argv[1], argv[2]);
	xyz->unit_type_size++;
	return 0;
}

int static loadNation(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int id = atoi(argv[0]);
	xyz->nations[id] = new db_nation(id, argv[1]);
	xyz->nation_size++;
	return 0;
}

int static loadResource(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int id = atoi(argv[0]);
	xyz->resources[id] = new db_resource(id, argv[1], argv[2], atoi(argv[3]), argv[4], argv[5], atof(argv[6]), atof(argv[7]));
	xyz->resource_size++;

	return 0;
}

int static loadHudVars(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int id = atoi(argv[0]);
	xyz->hudVars[id] = new db_hud_vars(id, atoi(argv[1]), argv[2], atof(argv[3]));
	xyz->hud_vars_size++;

	return 0;
}

int static loadBuildingToUnit(void* data, int argc, char** argv, char** azColName) {
	db_container* xyz = (db_container *)data;
	int buildingId = atoi(argv[1]);
	xyz->unitsForBuilding[buildingId]->push_back(xyz->units[atoi(argv[2])]);

	return 0;
}
void DatabaseCache::ifError(int rc, char* error) {
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", error);
		sqlite3_free(error);
	}
}

void DatabaseCache::execute(char* sqlUnits, int (* callback)(void*, int, char**, char**), db_container* dbContainer) {
	char* error;
	int rc = sqlite3_exec(database, sqlUnits, callback, dbContainer, &error);
	ifError(rc, error);
}

DatabaseCache::DatabaseCache() {
	int rc = sqlite3_open("Data/Database/base.db", &database);
	if (rc) {
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << endl << endl;
		sqlite3_close(database);
	}

	dbContainer = new db_container();

	execute("SELECT * from units", loadUnits, dbContainer);
	execute("SELECT * from hud_size", loadHudSizes, dbContainer);
	execute("SELECT * from graph_settings", loadGraphSettings, dbContainer);
	execute("SELECT * from building", loadBuildings, dbContainer);
	execute("SELECT * from building_type", loadBuildingType, dbContainer);
	execute("SELECT * from unit_type", loadUnitType, dbContainer);
	execute("SELECT * from nation", loadNation, dbContainer);
	execute("SELECT * from resource", loadResource, dbContainer);
	execute("SELECT * from hud_size_vars", loadHudVars, dbContainer);
	execute("SELECT * from building_to_unit", loadBuildingToUnit, dbContainer);


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

db_building_type* DatabaseCache::getBuildingType(int i) {
	return dbContainer->buildingTypes[i];
}

db_unit_type* DatabaseCache::getUnitType(int i) {
	return dbContainer->unitTypes[i];
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

int DatabaseCache::getResourceSize() {
	return dbContainer->resource_size;
}

int DatabaseCache::getBuildingTypeSize() {
	return dbContainer->building_type_size;
}

int DatabaseCache::getUnitTypeSize() {
	return dbContainer->unit_type_size;
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

std::vector<db_unit*>* DatabaseCache::getUnitsForBuilding(int id) {
	return dbContainer->unitsForBuilding[id];
}
