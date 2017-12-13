#include "SceneSaver.h"
#include <sqlite3/sqlite3.h>
#include <ostream>
#include <iostream>
#include <string>
#include "Game.h"
#include "player/PlayersManager.h"
#include "database/db_units.h"
#include "Loading.h"

SceneSaver::SceneSaver(int _precision) {
	//TODO zapisywanie powinno byc tylko miedzy klatkami
	precision = _precision;
	loadingState = new loading();
}


SceneSaver::~SceneSaver() {
	delete loadingState;
}

void SceneSaver::createUnitsTable() {
	std::string sql = "CREATE TABLE units(" + Unit::getColumns() + ");";

	createTable(sql);
}

void SceneSaver::createTable(std::string sql) {
	const char* charSql = sql.c_str();
	char* error;
	int rc = sqlite3_exec(database, charSql, nullptr, nullptr, &error);
	ifError(rc, error);
}

void SceneSaver::createBuildingsTable() {
	std::string sql = "CREATE TABLE buildings(" + Building::getColumns() + ");";

	createTable(sql);
}

void SceneSaver::createResourceEntitiesTable() {
	std::string sql = "CREATE TABLE resource_entities(" + ResourceEntity::getColumns() + ");";

	createTable(sql);
}

void SceneSaver::createPlayerTable() {
	std::string sql = "CREATE TABLE players(" + PlayersManager::getColumns() + ");";
	createTable(sql);
}

void SceneSaver::createConfigTable() {
	std::string sql = "CREATE TABLE config("
		"precision		INT     NOT NULL,"
		"map			INT     NOT NULL"
		");";
	createTable(sql);
}

void SceneSaver::createResourceTable() {
	std::string sql = "CREATE TABLE resources(" + Resources::getColumns() + ");";
	createTable(sql);
}

void SceneSaver::createTables() {
	createUnitsTable();
	createBuildingsTable();
	createResourceEntitiesTable();
	createPlayerTable();
	createConfigTable();
	createResourceTable();
	//createAimsTable();
}

void SceneSaver::createDatabase(Urho3D::String fileName) {
	std::string name = std::string("saves/") + fileName.CString() + ".db";
	int rc = sqlite3_open(name.c_str(), &database);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close(database);
	}
}

void SceneSaver::createSave(Urho3D::String fileName) {
	loadingState->reset(7, "create database");
	createDatabase(fileName);
	loadingState->inc("create Tables");
	createTables();
}

void SceneSaver::executeInsert(std::string sqlstatement) {
	sqlstatement[sqlstatement.size() - 1] = ';';
	sqlite3_stmt* stmt;
	sqlite3_prepare(database, sqlstatement.c_str(), -1, &stmt, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

void SceneSaver::saveUnits(std::vector<Unit*>* units) {
	loadingState->inc("saving units");
	if (units->empty()) { return; }
	std::string sql = "INSERT INTO units VALUES ";
	for (auto unit : *units) {
		sql += " (" + unit->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::saveBuildings(std::vector<Building*>* buildings) {
	loadingState->inc("saving buildings");
	if (buildings->empty()) { return; }
	std::string sql = "INSERT INTO buildings VALUES ";
	for (auto building : *buildings) {
		sql += " (" + building->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::saveResourceEntities(std::vector<ResourceEntity*>* resources) {
	loadingState->inc("saving resource_entities");
	if (resources->empty()) { return; }
	std::string sql = "INSERT INTO resource_entities VALUES ";
	for (auto resourceEntity : *resources) {
		sql += " (" + resourceEntity->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::savePlayers(std::vector<Player*>& players) {
	loadingState->inc("saving players");
	if (players.empty()) { return; }
	std::string sql = "INSERT INTO players VALUES ";
	for (auto player : players) {
		sql += " (" + player->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::saveResources(const std::vector<Player*>& players) {
	loadingState->inc("saving resources");
	if (players.empty()) { return; }
	std::string sql = "INSERT INTO resources VALUES ";
	for (auto player : players) {
		Resources* resources = player->getResources();
		sql += resources->getValues(precision, player->getId());
	}
	executeInsert(sql);
}

void SceneSaver::saveConfig() {
	loadingState->inc("saving config");
	std::string sql = "INSERT INTO config VALUES (";
	sql += std::to_string(precision) + "," + "1" + "),";//TODO id mapy wpisac
	executeInsert(sql);
}

void SceneSaver::close() {
	sqlite3_close(database);
	loadingState->inc("");
}