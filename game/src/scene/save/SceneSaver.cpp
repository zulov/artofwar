#include "SceneSaver.h"

#include "SQLConsts.h"
#include "database/db_utils.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "utils/PrintUtils.h"

SceneSaver::SceneSaver(int precision) : loadingState(7), precision(precision) {
	//TODO zapisywanie powinno byc tylko miedzy klatkami
}

void SceneSaver::createTable(const std::string& sql) const {
	auto createSql = SQLConsts::CREATE_TABLE + sql;
	const char* charSql = createSql.c_str();

	std::cout << (createSql);
	char* error;
	const int rc = sqlite3_exec(database, charSql, nullptr, nullptr, &error);
	ifError(rc, error, createSql);
}

void SceneSaver::createTables() const {
	createTable(SQLConsts::UNIT_COL);
	createTable(SQLConsts::BUILDING_COL);
	createTable(SQLConsts::RESOURCE_COL);
	createTable(SQLConsts::PLAYER_COL);
	createTable(SQLConsts::CONFIG_COL);
	createTable(SQLConsts::PLAYER_RESOURCES_COL);
}

void SceneSaver::createDatabase(const Urho3D::String& fileName) {
	database = nullptr;
	std::string name = std::string("saves/") + fileName.CString() + ".db";
	if (const int rc = sqlite3_open(name.c_str(), &database)) {
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
	ifError(rc, NULL, sql);
	const int rc1 = sqlite3_step(stmt);
	ifError(rc1, NULL, sql);
	const int rc2 = sqlite3_finalize(stmt);
	ifError(rc2, NULL, sql);
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

void SceneSaver::saveResources(std::vector<ResourceEntity*>* resources) {
	loadingState.inc("saving resources");
	if (resources->empty()) { return; }
	std::string sql = "INSERT INTO resources VALUES ";
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

void SceneSaver::saveConfig(int mapId, int size) {
	loadingState.inc("saving config");
	std::string sql = "INSERT INTO config VALUES (";
	sql += std::to_string(precision) + "," + std::to_string(mapId) + "," + std::to_string(size) + "),";

	executeInsert(sql);
}

void SceneSaver::close() {
	sqlite3_close_v2(database);
	loadingState.inc("");
}
