#include "SceneSaver.h"

#include "SQLConsts.h"
#include "database/db_utils.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/PlayersManager.h"

SceneSaver::SceneSaver(int precision): loadingState(7), precision(precision) {
	//TODO zapisywanie powinno byc tylko miedzy klatkami
}

void SceneSaver::createUnitsTable() const {
	const std::string sql = "units(" + SQLConsts::UNIT_COL + ");";

	createTable(sql);
}

void SceneSaver::createTable(const std::string& sql) const {
	const char* charSql = (SQLConsts::CREATE_TABLE + sql).c_str();
	char* error;
	const int rc = sqlite3_exec(database, charSql, nullptr, nullptr, &error);
	ifError(rc, error);
}

void SceneSaver::createBuildingsTable() const {
	const std::string sql = "buildings(" + SQLConsts::BUILDING_COL + ");";

	createTable(sql);
}

void SceneSaver::createResourceEntitiesTable() const {
	const std::string sql = "resource_entities(" + SQLConsts::RESOURCE_COL + ");";

	createTable(sql);
}

void SceneSaver::createPlayerTable() const {
	const std::string sql = "players(" + SQLConsts::PLAYER_COL + ");";
	createTable(sql);
}

void SceneSaver::createConfigTable() const {
	const std::string sql = "config(" + SQLConsts::CONFIG_COL + ");";
	createTable(sql);
}

void SceneSaver::createResourceTable() const {
	const std::string sql = "resources(" + SQLConsts::PLAYER_RESOURCES_COL + ");";
	createTable(sql);
}

void SceneSaver::createTables() const {
	createUnitsTable();
	createBuildingsTable();
	createResourceEntitiesTable();
	createPlayerTable();
	createConfigTable();
	createResourceTable();
}

void SceneSaver::createDatabase(const Urho3D::String& fileName) {
	database = nullptr;
	std::string name = std::string("saves/") + fileName.CString() + ".db";
	const int rc = sqlite3_open(name.c_str(), &database);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close_v2(database);
	}
}

void SceneSaver::createSave(const Urho3D::String& fileName) {
	loadingState.reset("create database");
	createDatabase(fileName);
	loadingState.inc("create Tables");
	createTables();
}

void SceneSaver::executeInsert(std::string& sql) const {
	sql[sql.size() - 1] = ';';
	sqlite3_stmt* stmt;
	const int rc = sqlite3_prepare(database, sql.c_str(), -1, &stmt, NULL);
	ifError(rc, NULL);
	const int rc1 = sqlite3_step(stmt);
	ifError(rc1, NULL);
	const int rc2 = sqlite3_finalize(stmt);
	ifError(rc2, NULL);
}

void SceneSaver::saveUnits(std::vector<Unit*>* units) {
	loadingState.inc("saving units");
	if (units->empty()) { return; }
	std::string sql = "INSERT INTO units VALUES ";
	for (auto unit : *units) {
		sql += " (" + unit->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::saveBuildings(std::vector<Building*>* buildings) {
	loadingState.inc("saving buildings");
	if (buildings->empty()) { return; }
	std::string sql = "INSERT INTO buildings VALUES ";
	for (auto building : *buildings) {
		sql += " (" + building->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::saveResourceEntities(std::vector<ResourceEntity*>* resources) {
	loadingState.inc("saving resource_entities");
	if (resources->empty()) { return; }
	std::string sql = "INSERT INTO resource_entities VALUES ";
	for (auto resourceEntity : *resources) {
		sql += " (" + resourceEntity->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::savePlayers(std::vector<Player*>& players) {
	loadingState.inc("saving players");
	if (players.empty()) { return; }
	std::string sql = "INSERT INTO players VALUES ";
	for (auto player : players) {
		sql += " (" + player->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::saveResources(const std::vector<Player*>& players) {
	loadingState.inc("saving resources");
	if (players.empty()) { return; }
	std::string sql = "INSERT INTO resources VALUES ";
	for (auto player : players) {
		Resources& resources = player->getResources();
		sql += resources.getValues(precision, player->getId());
	}
	executeInsert(sql);
}

void SceneSaver::saveConfig() {
	loadingState.inc("saving config");
	std::string sql = "INSERT INTO config VALUES (";
	sql += std::to_string(precision) + "," + "1" + "),"; //TODO id mapy wpisac
	executeInsert(sql);
}

void SceneSaver::close() {
	sqlite3_close_v2(database);
	loadingState.inc("");
}
