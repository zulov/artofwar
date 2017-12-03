#include "SceneSaver.h"
#include <sqlite3/sqlite3.h>
#include <ostream>
#include <iostream>
#include <string>


SceneSaver::SceneSaver(int _precision) {//TODO zapisywanie powinno byc tylko miedzy klatkami
	precision = _precision;
	loadingState = new loading();
}


SceneSaver::~SceneSaver() {
}

void SceneSaver::createTables() {
	const char* sql = "CREATE TABLE units("
		"id				INT		NOT NULL,"
		"position_x		INT     NOT NULL,"
		"position_z		INT     NOT NULL,"
		"state			INT     NOT NULL,"
		"velocity_x		INT     NOT NULL,"//TODO czy dodac y?
		"velocity_z		INT     NOT NULL,"
		"hp_coef		INT     NOT NULL);";

	rc = sqlite3_exec(database, sql, nullptr, nullptr, nullptr);

	sql = "CREATE TABLE buildings("
		"id				INT		NOT NULL,"
		"position_x		INT     NOT NULL,"
		"position_z		INT     NOT NULL);";

	rc = sqlite3_exec(database, sql, nullptr, nullptr, nullptr);
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
	loadingState->reset(2);
	createDatabase(fileName);
	loadingState->inc();
	createTables();
	loadingState->inc();
}

void SceneSaver::saveUnits(std::vector<Unit*>* units) {

	string sqlstatement = "INSERT INTO units VALUES ";
	for (auto unit : *units) {
		int id = unit->getID();
		int position_x = unit->getPosition()->x_ * precision;
		int position_z = unit->getPosition()->z_ * precision;
		int state = (int)unit->getState();
		int velocity_x = unit->getVelocity()->x_ * precision;
		int velocity_z = unit->getVelocity()->z_ * precision;
		int hp_coef = unit->getHealthPercent()* precision;
		sqlstatement += " (" + to_string(id)
			+ "," + to_string(position_x)
			+ "," + to_string(position_z)
			+ "," + to_string(state)
			+ "," + to_string(velocity_x)
			+ "," + to_string(velocity_z)
			+ "," + to_string(hp_coef)
			+ "),";

	}
	sqlstatement[sqlstatement.size() - 1] = ';';
	sqlite3_stmt* stmt;
	sqlite3_prepare(database, sqlstatement.c_str(), -1, &stmt, NULL);//preparing the statement
	sqlite3_step(stmt);//executing the statement
	sqlite3_finalize(stmt);
	sqlite3_close(database);
	loadingState->inc();
}

void SceneSaver::saveBuildings(std::vector<Building*>* buildings) {

}

void SceneSaver::saveResources(std::vector<ResourceEntity*>* resources) {

}
