#include "SceneLoader.h"
#include "dbload_container.h"
#include "database/db_utils.h"
#include "scene/save/SQLConsts.h"
#include "utils/StringUtils.h"

SceneLoader::~SceneLoader() { end(); }

void SceneLoader::load() {
	loadPlayers();

	loadUnits();
	loadBuildings();
	loadResourcesEntities();
	close();
}

int static load_config(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto xyz = static_cast<dbload_container*>(data);
	xyz->precision = atoi(argv[0]);
	xyz->config->precision = xyz->precision;
	xyz->config->map = atoi(argv[1]);
	xyz->config->size = atoi(argv[2]);

	return 0;
}

int static load_units(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto xyz = static_cast<dbload_container*>(data);
	int p = xyz->precision;
	xyz->units->push_back(new dbload_unit(atoi(argv[0]), atof(argv[1]) / p, atoi(argv[2]), atoi(argv[3]),
	                                      atoi(argv[4]), atof(argv[5]) / p, atof(argv[6]) / p,
	                                      atoi(argv[7]), atof(argv[8]) / p, atof(argv[9]) / p));

	return 0;
}

int static load_resources_entities(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto xyz = static_cast<dbload_container*>(data);
	float p = xyz->precision;
	xyz->resources->push_back(new dbload_resource(fatoi(argv[0]), fatoi(argv[1]) / p, fatoi(argv[2]),
	                                              fatoi(argv[3]), fatoi(argv[4]),
	                                              fatoi(argv[5]), fatoi(argv[6])));

	return 0;
}

int static load_resources_entities_size(void* data, int argc, char** argv, char** azColName) {
	if (argc == 0) { return 0; }
	const auto xyz = static_cast<dbload_container*>(data);

	xyz->resources->reserve(atoi(argv[0]));

	return 0;
}

void SceneLoader::reset() {
	database = nullptr;
	delete dbLoad;
	dbLoad = new dbload_container();
}

dbload_container* SceneLoader::getData() const { return dbLoad; }

void SceneLoader::createLoad(const Urho3D::String& fileName, bool tryReuse) {
	if (fileName == lastLoad && tryReuse) { return; }
	reset();

	lastLoad = fileName;
	std::string name = std::string("saves/") + fileName.CString();
	int rc = sqlite3_open_v2(name.c_str(), &database, SQLITE_OPEN_READONLY, nullptr);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close_v2(database);
	}
	load(SQLConsts::SELECT + "config", load_config);
}

dbload_config* SceneLoader::getConfig() const { return dbLoad->config; }

const std::vector<dbload_player*>* SceneLoader::loadPlayers() const {
	if (dbLoad->players) { return dbLoad->players; }
	dbLoad->players = new std::vector<dbload_player*>();

	load("players", [this](auto* s) { dbLoad->players->push_back(new dbload_player(s, dbLoad->precision)); });
	return dbLoad->players;
}

void SceneLoader::loadUnits() const {
	if (dbLoad->units) { return; }
	dbLoad->units = new std::vector<dbload_unit*>();
	load("units", [this](auto* s) { dbLoad->units->push_back(new dbload_unit(s, dbLoad->precision)); });
}

void SceneLoader::loadBuildings() const {
	if (dbLoad->buildings) { return; }
	dbLoad->buildings = new std::vector<dbload_building*>();

	load("buildings", [this](auto* s) { dbLoad->buildings->push_back(new dbload_building(s, dbLoad->precision)); });
}

void SceneLoader::loadResourcesEntities() const {
	if (dbLoad->resources) { return; }
	dbLoad->resources = new std::vector<dbload_resource*>();

	load(SQLConsts::COUNT + "resources", load_resources_entities_size);
	load("resources", load_resources_entities);
}

//
// void SceneLoader::load(const std::string& sql, int (*load)(void*, int, char**, char**)) const {
// 	char* error;
// 	int rc = sqlite3_exec(database, sql.c_str(), load, dbLoad, &error);
// 	ifError(rc, error, sql);
// }

template <typename Creator>
void SceneLoader::load(const std::string& tableName, Creator createFn) const {
	std::string sqlStr = SQLConsts::SELECT + tableName;
	const char* sql = sqlStr.c_str();
	sqlite3_stmt* stmt = nullptr;

	if (sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr) != SQLITE_OK) {
		// obs³uga b³êdu
		return;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) { createFn(stmt); }

	sqlite3_finalize(stmt);
}

void SceneLoader::end() {
	close();
	//delete dbLoad;
	//dbLoad = nullptr;
}

void SceneLoader::close() {
	sqlite3_close_v2(database);
	database = nullptr;
}
