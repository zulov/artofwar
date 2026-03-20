#include "SceneLoader.h"
#include "dbload_container.h"
#include "database/db_utils.h"
#include "utils/StringUtils.h"

SceneLoader::~SceneLoader() { end(); }

void SceneLoader::load() {
	loadPlayers();

	loadUnits();
	loadBuildings();
	loadResourcesEntities();
	close();
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
	load("config", [this](auto* s) { dbLoad->config = new dbload_config(s); });
}

dbload_config* SceneLoader::getConfig() const { return dbLoad->config; }

const std::vector<dbload_player*>* SceneLoader::loadPlayers() const {
	if (dbLoad->players) { return dbLoad->players; }
	dbLoad->players = new std::vector<dbload_player*>();

	load("players",
		 [this](auto* s) { dbLoad->players->push_back(new dbload_player(s, dbLoad->config->precision)); });
	return dbLoad->players;
}

void SceneLoader::loadUnits() const {
	if (dbLoad->units) { return; }
	dbLoad->units = new std::vector<dbload_unit*>();
	load("units",
		 [this](auto* s) { dbLoad->units->push_back(new dbload_unit(s, dbLoad->config->precision)); });
}

void SceneLoader::loadBuildings() const {
	if (dbLoad->buildings) { return; }
	dbLoad->buildings = new std::vector<dbload_building*>();

	load("buildings",
		 [this](auto* s) { dbLoad->buildings->push_back(new dbload_building(s, dbLoad->config->precision)); });
}

void SceneLoader::loadResourcesEntities() const {
	if (dbLoad->resources) { return; }
	dbLoad->resources = new std::vector<dbload_resource*>();

	count("resources", [this](auto* s) { dbLoad->resources->reserve(asInt(s, 0)); });
	load("resources",
		 [this](auto* s) { dbLoad->resources->push_back(new dbload_resource(s, dbLoad->config->precision)); });
}

void SceneLoader::end() {
	close();
}

void SceneLoader::close() {
	sqlite3_close_v2(database);
	database = nullptr;
}
