#include "SceneSaver.h"
#include <sqlite3/sqlite3.h>
#include <ostream>
#include <iostream>
#include <string>


SceneSaver::SceneSaver(int _precision) {
	//TODO zapisywanie powinno byc tylko miedzy klatkami
	precision = _precision;
	loadingState = new loading();
}


SceneSaver::~SceneSaver() {
	delete loadingState;
}

void SceneSaver::createUnitsTable() {
	string sql = "CREATE TABLE units(" + Unit::getColumns() + ");";

	createTable(sql);
}

void SceneSaver::createTable(string sql) {
	const char* charSql = sql.c_str();
	rc = sqlite3_exec(database, charSql, nullptr, nullptr, nullptr);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << charSql << std::endl;
		sqlite3_close(database);
	}
}

void SceneSaver::createBuildingsTable() {
	string sql = "CREATE TABLE buildings(" + Building::getColumns() + ");";

	createTable(sql);
}

void SceneSaver::createResourceTable() {
	string sql = "CREATE TABLE resources(" + ResourceEntity::getColumns() + ");";

	createTable(sql);
}

void SceneSaver::createTables() {
	createUnitsTable();
	createBuildingsTable();
	createResourceTable();
}

void SceneSaver::createDatabase(Urho3D::String fileName) {
	std::string name = std::string("saves/") + fileName.CString() + ".db";
	rc = sqlite3_open(name.c_str(), &database);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close(database);
	}
}

void SceneSaver::createSave(Urho3D::String fileName) {
	loadingState->reset(3, "create database");
	createDatabase(fileName);
	loadingState->inc("create Tables");
	createTables();
}

void SceneSaver::executeInsert(string sqlstatement) {
	sqlstatement[sqlstatement.size() - 1] = ';';
	sqlite3_stmt* stmt;
	sqlite3_prepare(database, sqlstatement.c_str(), -1, &stmt, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

void SceneSaver::saveUnits(std::vector<Unit*>* units) {
	loadingState->inc("saving units");
	if (units->empty()) { return; }
	string sql = "INSERT INTO units VALUES ";
	for (auto unit : *units) {
		sql += " (" + unit->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::saveBuildings(std::vector<Building*>* buildings) {
	loadingState->inc("saving buildings");
	if (buildings->empty()) { return; }
	string sql = "INSERT INTO buildings VALUES ";
	for (auto building : *buildings) {
		sql += " (" + building->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::saveResources(std::vector<ResourceEntity*>* resources) {
	loadingState->inc("saving resources");
	if (resources->empty()) { return; }
	string sql = "INSERT INTO resources VALUES ";
	for (auto resourceEntity : *resources) {
		sql += " (" + resourceEntity->getValues(precision) + "),";
	}
	executeInsert(sql);
}

void SceneSaver::close() {
	sqlite3_close(database);
	loadingState->inc("");
}
