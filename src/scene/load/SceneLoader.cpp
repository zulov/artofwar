#include "SceneLoader.h"
#include <sqlite3/sqlite3.h>
#include <ostream>
#include <iostream>
#include "Loading.h"
#include "database/db_units.h"
#include "db_load.h"


SceneLoader::SceneLoader() {
	loadingState = new loading();
}


SceneLoader::~SceneLoader() {
	delete loadingState;
}

int static loadConfig(void* data, int argc, char** argv, char** azColName) {
	db_load* xyz = (db_load *)data;
	xyz->config->precision = atoi(argv[0]);
	xyz->config->map = atoi(argv[1]);
	return 0;
}


void SceneLoader::createLoad(Urho3D::String fileName) {
	loadingState->reset(3, "start loading");
	dbLoad = new db_load();
	std::string name = std::string("saves/") + fileName.CString() + ".db";
	int rc = sqlite3_open(name.c_str(), &database);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close(database);
	}
	loadingState->inc("load config");
	load("SELECT * from config", loadConfig);
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
