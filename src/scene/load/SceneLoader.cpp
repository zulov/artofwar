#include "SceneLoader.h"
#include "dbload_container.h"
#include "database/db_utils.h"


SceneLoader::SceneLoader(): loadingState(3), dbLoad(nullptr) {}


SceneLoader::~SceneLoader() {
	delete dbLoad;
}

void SceneLoader::load() {
	loadPlayers();
	loadResources();

	loadUnits();
	loadBuildings();
	loadResourcesEntities();
}


int static load_config(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<dbload_container*>(data);
	xyz->config->precision = atoi(argv[0]);
	xyz->config->map = atoi(argv[1]);
	xyz->precision = atoi(argv[0]);
	return 0;
}

int static load_players(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<dbload_container*>(data);
	xyz->players->push_back(new dbload_player(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]),
	                                          atoi(argv[3]), argv[4], atoi(argv[5])));

	return 0;
}

int static load_resources(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<dbload_container*>(data);
	int p = xyz->precision;
	xyz->resources->push_back(new dbload_resource(atoi(argv[0]), atoi(argv[1]), atof(argv[2]) / p));

	return 0;
}

int static load_units(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<dbload_container*>(data);
	int p = xyz->precision;
	xyz->units->push_back(new dbload_unit(atoi(argv[0]), atof(argv[1]) / p, atoi(argv[2]),
	                                      atoi(argv[3]), atof(argv[4]) / p, atof(argv[5]) / p,
	                                      atoi(argv[6]), atof(argv[7]) / p, atof(argv[8]) / p));

	return 0;
}

int static load_buildings(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<dbload_container*>(data);
	int p = xyz->precision;
	xyz->buildings->push_back(new dbload_building(atoi(argv[0]), atof(argv[1]) / p, atoi(argv[2]),
	                                              atoi(argv[3]), atoi(argv[4]), atoi(argv[5]),
	                                              atoi(argv[6]), atoi(argv[7]), atoi(argv[8])));

	return 0;
}

int static load_resources_entities(void* data, int argc, char** argv, char** azColName) {
	const auto xyz = static_cast<dbload_container*>(data);
	int p = xyz->precision;
	xyz->resource_entities->push_back(new dbload_resource_entities(atoi(argv[0]), atof(argv[1]) / p, atoi(argv[2]),
	                                                               atoi(argv[3]), atoi(argv[4]), atoi(argv[5]),
	                                                               atoi(argv[6]), atoi(argv[7])));

	return 0;
}

void SceneLoader::reset() {
	database = nullptr;
	delete dbLoad;
	dbLoad = new dbload_container();
	loadingState.reset("start loading");
}

dbload_container* SceneLoader::getData() const {
	return dbLoad;
}


void SceneLoader::createLoad(const Urho3D::String& fileName) {
	reset();

	std::string name = std::string("saves/") + fileName.CString();
	int rc = sqlite3_open_v2(name.c_str(), &database, SQLITE_OPEN_READONLY, nullptr);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close(database);
	}
	loadingState.inc("load config");
	load("SELECT * from config", load_config);
}

std::vector<dbload_player*>* SceneLoader::loadPlayers() {
	loadingState.inc("load players");
	load("SELECT * from players", load_players);
	return dbLoad->players;
}

std::vector<dbload_resource*>* SceneLoader::loadResources() {
	loadingState.inc("load resources");
	load("SELECT * from resources", load_resources);
	return dbLoad->resources;
}

std::vector<dbload_unit*>* SceneLoader::loadUnits() {
	loadingState.inc("load units");
	load("SELECT * from units", load_units);
	return dbLoad->units;
}

std::vector<dbload_building*>* SceneLoader::loadBuildings() {
	loadingState.inc("load buildings");
	load("SELECT * from buildings", load_buildings);
	return dbLoad->buildings;
}

std::vector<dbload_resource_entities*>* SceneLoader::loadResourcesEntities() {
	loadingState.inc("load resource_entities");
	load("SELECT * from resource_entities", load_resources_entities);
	return dbLoad->resource_entities;
}

void SceneLoader::load(const char* sql, int (*load)(void*, int, char**, char**)) const {
	char* error;
	int rc = sqlite3_exec(database, sql, load, dbLoad, &error);
	ifError(rc, error);
}

void SceneLoader::end() {
	sqlite3_close(database);
	delete dbLoad;
	dbLoad = nullptr;
	loadingState.inc("");
}
