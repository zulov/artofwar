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
	precision(precision), savingProgress(5) {
	//TODO zapisywanie powinno byc tylko miedzy klatkami
}

void SceneSaver::createDatabase(const Urho3D::String& fileName) {
	database = nullptr;
	std::string name = std::string("saves/") + fileName.CString() + ".db";
	if (const int rc = sqlite3_open(name.c_str(), &database)) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << "\n\n";
		sqlite3_close_v2(database);
		return;
	}

	auto exec = [&](const char* sql) {
		char* err = nullptr;
		int rc = sqlite3_exec(database, sql, nullptr, nullptr, &err);
		if (rc != SQLITE_OK) { sqlite3_free(err); }
	};

    exec("PRAGMA journal_mode = DELETE;");
	exec("PRAGMA synchronous = NORMAL;");
	exec("PRAGMA temp_store = MEMORY;");
	exec("PRAGMA page_size = 1024");
}

void SceneSaver::createTable(const std::string& name, const std::string& sql) {
	savingProgress.inc(std::format("saving {}", name));
	auto createSql = SQLConsts::CREATE_TABLE + name + sql;
	const char* charSql = createSql.c_str();

	char* error;
	const int rc = sqlite3_exec(database, charSql, nullptr, nullptr, &error);
	ifError(rc, error, createSql);
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
	finalizeDatabase();
	sqlite3_exec(database, "COMMIT;", nullptr, nullptr, nullptr);
	close();
}

void SceneSaver::finalizeDatabase() {
	char* err = nullptr;
	sqlite3_exec(database, "VACUUM;", nullptr, nullptr, &err);
	if (err) {
		sqlite3_free(err);
	}
}

void SceneSaver::saveUnits(const std::vector<Unit*>* units) {
	createTable(SQLConsts::UNIT_NAME, SQLConsts::UNIT_COL);

	if (!units || units->empty()) { return; }
	const auto sql = make_insert_sql("units", unit_columns);
	const auto params = prefix_with_colon(unit_columns);

	executeBatch(database, sql.c_str(), [&](sqlite3_stmt* stmt) {
		ParamMap param(stmt, params);

		for (auto* u : *units) {
			bindRow(stmt, param, precision, u);
			stepAndReset(stmt, sql.c_str());
		}
	});
}

void SceneSaver::saveBuildings(const std::vector<Building*>* buildings) {
	createTable(SQLConsts::BUILDING_NAME, SQLConsts::BUILDING_COL);

	if (!buildings || buildings->empty()) { return; }

	const auto sql = make_insert_sql("buildings", building_columns);
	const auto params = prefix_with_colon(building_columns);

	executeBatch(database, sql.c_str(), [&](sqlite3_stmt* stmt) {
		ParamMap param(stmt, params);

		for (auto* b : *buildings) {
			bindRow(stmt, param, precision, b);
			stepAndReset(stmt, sql.c_str());
		}
	});
}

void SceneSaver::saveResources(const std::vector<ResourceEntity*>* resources) {
	createTable(SQLConsts::RESOURCE_NAME, SQLConsts::RESOURCE_COL);

	if (!resources || resources->empty()) { return; }

	const auto sql = make_insert_sql("resources", resources_columns);
	const auto params = prefix_with_colon(resources_columns);

	executeBatch(database, sql.c_str(), [&](sqlite3_stmt* stmt) {
		ParamMap param(stmt, params);

		for (auto* r : *resources) {
			bindRow(stmt, param, precision, r);
			stepAndReset(stmt, sql.c_str());
		}
	});
}

void SceneSaver::savePlayers(const std::vector<Player*>& players) {
	createTable(SQLConsts::PLAYER_NAME, SQLConsts::PLAYER_COL);

	if (players.empty()) { return; }

	const auto sql = make_insert_sql("players", players_columns);
	const auto params = prefix_with_colon(players_columns);

	executeBatch(database, sql.c_str(), [&](sqlite3_stmt* stmt) {
		ParamMap param(stmt, params);

		for (auto* p : players) {
			bindRow(stmt, param, precision, p);
			stepAndReset(stmt, sql.c_str());
		}
	});
}

void SceneSaver::saveConfig(int mapId, int size) {
	createTable(SQLConsts::CONFIG_NAME, SQLConsts::CONFIG_COL);

	const auto sql = make_insert_sql("config", config_columns);
	const auto params = prefix_with_colon(config_columns);

	executeBatch(database, sql.c_str(), [&](sqlite3_stmt* stmt) {
		ParamMap p(stmt, params);
		bindI(stmt, p[":precision"], precision);
		bindI(stmt, p[":map"], mapId);
		bindI(stmt, p[":size"], size);

		stepAndReset(stmt, sql.c_str());
	});
}

void SceneSaver::close() {
	sqlite3_close_v2(database);
	savingProgress.inc("");
}
