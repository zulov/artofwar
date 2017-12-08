#include "SceneLoader.h"
#include <sqlite3/sqlite3.h>
#include <ostream>
#include <iostream>
#include "Loading.h"
#include "database/db_units.h"
#include "dbload_load.h"


SceneLoader::SceneLoader() {
	loadingState = new loading();
	dbLoad = nullptr;
}


SceneLoader::~SceneLoader() {
	delete loadingState;
}


int static load_config(void* data, int argc, char** argv, char** azColName) {
	dbload_load* xyz = (dbload_load *)data;
	xyz->config->precision = atoi(argv[0]);
	xyz->config->map = atoi(argv[1]);
	xyz->precision = atoi(argv[0]);
	return 0;
}

int static load_players(void* data, int argc, char** argv, char** azColName) {
	dbload_load* xyz = (dbload_load *)data;
	int p = xyz->precision;
	xyz->players->push_back(new dbload_player(
		atoi(argv[0]), atoi(argv[1])
	));

	return 0;
}

int static load_resources(void* data, int argc, char** argv, char** azColName) {
	dbload_load* xyz = (dbload_load *)data;
	int p = xyz->precision;
	xyz->resources->push_back(new dbload_resource(
		atoi(argv[0]), atoi(argv[1]), atof(argv[2]) / p
	));

	return 0;
}

int static load_units(void* data, int argc, char** argv, char** azColName) {
	dbload_load* xyz = (dbload_load *)data;
	int p = xyz->precision;
	xyz->units->push_back(new dbload_unit(
		atoi(argv[0]), atoi(argv[1]), atof(argv[2]) / p,
		atoi(argv[3]), atof(argv[4]) / p, atof(argv[5]) / p,
		atoi(argv[6]), atof(argv[7]) / p, atof(argv[8]) / p,
		atoi(argv[9])
		));

	return 0;
}

int static load_buildings(void* data, int argc, char** argv, char** azColName) {
	dbload_load* xyz = (dbload_load *)data;
	int p = xyz->precision;
	xyz->buildings->push_back(new dbload_building(
		atoi(argv[0]), atoi(argv[1]), atof(argv[2]) / p,
		atoi(argv[3]), atoi(argv[4]), atoi(argv[5]),
		atof(argv[6]) / p, atof(argv[7]) / p
	));

	return 0;
}

int static load_resources_entities(void* data, int argc, char** argv, char** azColName) {
	dbload_load* xyz = (dbload_load *)data;
	int p = xyz->precision;
	xyz->resource_entities->push_back(new dbload_resource_entities(
		atoi(argv[0]), atoi(argv[1]), atof(argv[2]) / p,
		atoi(argv[3]), atoi(argv[4]), atoi(argv[5]),
		atof(argv[6]) / p
	));

	return 0;
}

void SceneLoader::reset() {
	if (dbLoad) {
		delete dbLoad;
	}
	dbLoad = new dbload_load();
	loadingState->reset(3, "start loading");
}


void SceneLoader::createLoad(Urho3D::String fileName) {
	reset();

	std::string name = std::string("saves/") + fileName.CString() + ".db";
	int rc = sqlite3_open(name.c_str(), &database);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close(database);
	}
	loadingState->inc("load config");
	load("SELECT * from config", load_config);
}

std::vector<dbload_player*>* SceneLoader::loadPlayers() {
	loadingState->inc("load players");
	load("SELECT * from players", load_players);
	return dbLoad->players;
}

std::vector<dbload_resource*>* SceneLoader::loadResources() {
	loadingState->inc("load resources");
	load("SELECT * from resources", load_resources);
	return dbLoad->resources;
}

std::vector<dbload_unit*>* SceneLoader::loadUnits() {
	loadingState->inc("load units");
	load("SELECT * from units", load_units);
	return dbLoad->units;
}

std::vector<dbload_building*>* SceneLoader::loadBuildings() {
	loadingState->inc("load buildings");
	load("SELECT * from buildings", load_buildings);
	return dbLoad->buildings;
}

std::vector<dbload_resource_entities*>* SceneLoader::loadResourcesEntities() {
	loadingState->inc("load resource_entities");
	load("SELECT * from resource_entities", load_resources_entities);
	return dbLoad->resource_entities;
}

void SceneLoader::load(char* sql, int (*load)(void*, int, char**, char**)) {
	char* error;
	int rc = sqlite3_exec(database, sql, load, dbLoad, &error);
	ifError(rc, error);
}

void SceneLoader::end() {
	sqlite3_close(database);
	loadingState->inc("");
}
