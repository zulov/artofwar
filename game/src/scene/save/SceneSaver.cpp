#include "SceneSaver.h"

#include "SQLConsts.h"
#include "database/db_insert_defs.h"
#include "database/db_insert_utils.h"
#include "database/db_utils.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"

SceneSaver::SceneSaver(int precision) :
	precision(precision), savingProgress(6) {
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

void SceneSaver::createDatabase(const Urho3D::String& fileName) {
	database = nullptr;
	std::string name = std::string("saves/") + fileName.CString() + ".db";
	if (const int rc = sqlite3_open(name.c_str(), &database)) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << std::endl << std::endl;
		sqlite3_close_v2(database);
	}
}

void SceneSaver::createSave(const Urho3D::String& fileName, const std::vector<Unit*>* units,
							const std::vector<Building*>* buildings, const std::vector<ResourceEntity*>* resources,
                            const std::vector<Player*>& players, int mapId, int size) {
	savingProgress.reset("create database");

	createDatabase(fileName);

	sqlite3_exec(database, "BEGIN;", nullptr, nullptr, nullptr);

	saveUnits(units);
	saveBuildings(buildings);
	saveResources(resources);
	savePlayers(players);
	saveConfig(mapId, size);

	sqlite3_exec(database, "COMMIT;", nullptr, nullptr, nullptr);
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

void SceneSaver::saveUnits(const std::vector<Unit*>* units) {
	savingProgress.inc("saving units");
	createTable(SQLConsts::UNIT_COL);

	if (!units || units->empty()) { return; }

	const auto sql = make_insert_sql("units", unit_columns).c_str();
	const auto params = prefix_with_colon(unit_columns);

	executeBatch(database, sql, [&](sqlite3_stmt* stmt) {
		ParamMap param(stmt, params);

		for (auto* u : *units) {
			bindRow(stmt, param, precision, u);
			stepAndReset(stmt, sql);
		}
	});
}

void SceneSaver::saveBuildings(const std::vector<Building*>* buildings) {
	savingProgress.inc("saving buildings");
	createTable(SQLConsts::BUILDING_COL);

	if (!buildings || buildings->empty()) { return; }

	const auto sql = make_insert_sql("buildings", building_columns).c_str();
	
	const auto params = prefix_with_colon(building_columns);

	executeBatch(database, sql, [&](sqlite3_stmt* stmt) {
		ParamMap param(stmt, params);

		for (auto* b : *buildings) {
			bindRow(stmt, param, precision, b);
			stepAndReset(stmt, sql);
		}
	});
}

void SceneSaver::saveResources(const std::vector<ResourceEntity*>* resources) {
	savingProgress.inc("saving resources");
	createTable(SQLConsts::RESOURCE_COL);

	if (!resources || resources->empty()) { return; }

	const auto sql = make_insert_sql("resources", resources_columns).c_str();
	const auto params = prefix_with_colon(resources_columns);

	executeBatch(database, sql, [&](sqlite3_stmt* stmt) {
		ParamMap param(stmt, params);

		for (auto* r : *resources) {
			bindRow(stmt, param, precision, r);
			stepAndReset(stmt, sql);
		}
	});
}

void SceneSaver::savePlayers(const std::vector<Player*>& players) {
	savingProgress.inc("saving players");
	createTable(SQLConsts::PLAYER_COL);

	if (players.empty()) { return; }

	const auto sql = make_insert_sql("players", players_columns).c_str();
	const auto params = prefix_with_colon(players_columns);

	executeBatch(database, sql, [&](sqlite3_stmt* stmt) {
		ParamMap param(stmt, params);

		for (auto* p : players) {
			bindRow(stmt, param, precision, p);
			stepAndReset(stmt, sql);
		}
	});
}

void SceneSaver::saveConfig(int mapId, int size) {
	savingProgress.inc("saving config");
	createTable(SQLConsts::CONFIG_COL);

	const auto sql = make_insert_sql("config", config_columns).c_str();
	const auto params = prefix_with_colon(config_columns);

	executeBatch(database, sql, [&](sqlite3_stmt* stmt) {
		ParamMap p(stmt, params);
		bindI(stmt, p[":precision"], precision);
		bindI(stmt, p[":map"], mapId);
		bindI(stmt, p[":size"], size);

		stepAndReset(stmt, sql);
	});
}

void SceneSaver::close() {
	sqlite3_close_v2(database);
	savingProgress.inc("");
}
