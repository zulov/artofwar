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
	string sql = "CREATE TABLE units(" +
		Unit::getColumns() +
		");";

	rc = sqlite3_exec(database, sql.c_str(), nullptr, nullptr, nullptr);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << sql.c_str() << std::endl;
		sqlite3_close(database);
	}
}

void SceneSaver::createBuildingsTable() {
	string sql = "CREATE TABLE buildings(" + Building::getColumns() + ");";

	rc = sqlite3_exec(database, sql.c_str(), nullptr, nullptr, nullptr);
	if (rc) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << sql.c_str() << std::endl;
		sqlite3_close(database);
	}
}

void SceneSaver::createTables() {
	createUnitsTable();
	createBuildingsTable();
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

void SceneSaver::saveUnits(std::vector<Unit*>* units) {
	loadingState->inc("saving units");
	string sqlstatement = "INSERT INTO units VALUES ";
	for (auto unit : *units) {

		sqlstatement += " (" + unit->getValues(precision) + "),";

	}
	sqlstatement[sqlstatement.size() - 1] = ';';
	sqlite3_stmt* stmt;
	sqlite3_prepare(database, sqlstatement.c_str(), -1, &stmt, NULL);//preparing the statement
	sqlite3_step(stmt);//executing the statement
	sqlite3_finalize(stmt);

}

void SceneSaver::saveBuildings(std::vector<Building*>* buildings) {
	loadingState->inc("saving buildings");
}

void SceneSaver::saveResources(std::vector<ResourceEntity*>* resources) {
	loadingState->inc("saving resources");
}

void SceneSaver::close() {
	sqlite3_close(database);
	loadingState->inc("");
}
