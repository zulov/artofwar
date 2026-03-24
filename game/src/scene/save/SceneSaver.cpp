#include "SceneSaver.h"

#include "SQLConsts.h"
#include "database/db_insert_utils.h"
#include "database/db_utils.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"

SceneSaver::SceneSaver(int precision) : precision(precision), savingProgress(6) {
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

void SceneSaver::createSave(const Urho3D::String& fileName, std::vector<Unit*>* units,
							std::vector<Building*>* buildings, std::vector<ResourceEntity*>* resources,
							std::vector<Player*>& players, int mapId, int size) {
	savingProgress.reset("create database");

	createDatabase(fileName);

	sqlite3_exec(database, "BEGIN;", nullptr, nullptr, nullptr);

	saveUnits(units);
	saveBuildings(buildings);
	saveResources(resources);
	savePlayers(players);
	saveConfig(mapId,size);

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

void SceneSaver::saveUnits(std::vector<Unit*>* units) {
	savingProgress.inc("saving units");
	createTable(SQLConsts::UNIT_COL);
	if (!units || units->empty()) {return;}
	const char* sql = "INSERT INTO units (id_db, hp_coef, uid, player, level, position_x, position_z, state, velocity_x, velocity_z) "
				   "VALUES (:id_db, :hp_coef, :uid, :player, :level, :pos_x, :pos_z, :state, :vel_x, :vel_z);";

    executeBatch(database, sql, [&](sqlite3_stmt* stmt) {
		ParamMap p( stmt,
				{":id_db", ":hp_coef", ":uid", ":player", ":level", ":pos_x", ":pos_z", ":state", ":vel_x", ":vel_z"});

		for (auto u : *units) {
			bindRow(stmt, p, *u);
			stepAndReset(stmt, sql);
		}
	});
}

void SceneSaver::saveBuildings(std::vector<Building*>* buildings) {
	savingProgress.inc("saving buildings");
	createTable(SQLConsts::BUILDING_COL);

	if (!buildings || buildings->empty()) { return; }

    const char* sql = "INSERT INTO buildings (id_db, hp_coef, uid, player, level, bucket_x, bucket_y, state, next_state, deploy_Idx) "
					  "VALUES (:id_db, :hp_coef, :uid, :player, :level, :bx, :by, :state, :next_state, :deploy);";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr);
	ifError(rc, nullptr, sql);

	int id_db = sqlite3_bind_parameter_index(stmt, ":id_db");
	int hp = sqlite3_bind_parameter_index(stmt, ":hp_coef");
	int uid = sqlite3_bind_parameter_index(stmt, ":uid");
	int player = sqlite3_bind_parameter_index(stmt, ":player");
	int level = sqlite3_bind_parameter_index(stmt, ":level");
	int bx = sqlite3_bind_parameter_index(stmt, ":bx");
	int by = sqlite3_bind_parameter_index(stmt, ":by");
	int state = sqlite3_bind_parameter_index(stmt, ":state");
	int next = sqlite3_bind_parameter_index(stmt, ":next_state");
	int dep = sqlite3_bind_parameter_index(stmt, ":deploy");

	for (auto b : *buildings) {
		sqlite3_bind_int(stmt, id_db, b->getIdDb());
		sqlite3_bind_int(stmt, hp, b->getHpCoef());
		sqlite3_bind_int(stmt, uid, b->getUid());
		sqlite3_bind_int(stmt, player, b->getPlayer());
		sqlite3_bind_int(stmt, level, b->getLevel());
		sqlite3_bind_int(stmt, bx, b->getBucketX());
		sqlite3_bind_int(stmt, by, b->getBucketY());
		sqlite3_bind_int(stmt, state, b->getState());
		sqlite3_bind_int(stmt, next, b->getNextState());
		sqlite3_bind_int(stmt, dep, b->getDeployIdx());

		rc = sqlite3_step(stmt);
		ifError(rc, nullptr, sql);

		sqlite3_reset(stmt);
		sqlite3_clear_bindings(stmt);
	}

	sqlite3_finalize(stmt);
}

void SceneSaver::saveResources(std::vector<ResourceEntity*>* resources) {
	savingProgress.inc("saving resources");
	createTable(SQLConsts::RESOURCE_COL);

	if (!resources || resources->empty()) { return; }

	 const char* sql = "INSERT INTO resources (id_db, hp_coef, uid, bucket_x, bucket_y, state, next_state) "
					"VALUES (:id_db, :hp_coef, :uid, :bx, :by, :state, :next);";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr);
	ifError(rc, nullptr, sql);

	int id = sqlite3_bind_parameter_index(stmt, ":id_db");
	int hp = sqlite3_bind_parameter_index(stmt, ":hp_coef");
	int uid = sqlite3_bind_parameter_index(stmt, ":uid");
	int bx = sqlite3_bind_parameter_index(stmt, ":bx");
	int by = sqlite3_bind_parameter_index(stmt, ":by");
	int st = sqlite3_bind_parameter_index(stmt, ":state");
	int nx = sqlite3_bind_parameter_index(stmt, ":next");

	for (auto r : *resources) {
		sqlite3_bind_int(stmt, id, r->getIdDb());
		sqlite3_bind_int(stmt, hp, r->getHpCoef());
		sqlite3_bind_int(stmt, uid, r->getUid());
		sqlite3_bind_int(stmt, bx, r->getBucketX());
		sqlite3_bind_int(stmt, by, r->getBucketY());
		sqlite3_bind_int(stmt, st, r->getState());
		sqlite3_bind_int(stmt, nx, r->getNextState());

		rc = sqlite3_step(stmt);
		ifError(rc, nullptr, sql);

		sqlite3_reset(stmt);
		sqlite3_clear_bindings(stmt);
	}

	sqlite3_finalize(stmt);
}

void SceneSaver::savePlayers(std::vector<Player*>& players) {
	savingProgress.inc("saving players");
	createTable(SQLConsts::PLAYER_COL);
	if (players.empty())
		return;

    const char* sql = "INSERT INTO players (id, is_active, team, nation, name, color, buildingUid, unitUid, food, wood, stone, gold) "
					  "VALUES (:id, :active, :team, :nation, :name, :color, :buid, :uuid, :food, :wood, :stone, :gold);";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr);
	ifError(rc, nullptr, sql);

	int id = sqlite3_bind_parameter_index(stmt, ":id");
	int act = sqlite3_bind_parameter_index(stmt, ":active");
	int team = sqlite3_bind_parameter_index(stmt, ":team");
	int nat = sqlite3_bind_parameter_index(stmt, ":nation");
	int name = sqlite3_bind_parameter_index(stmt, ":name");
	int col = sqlite3_bind_parameter_index(stmt, ":color");
	int buid = sqlite3_bind_parameter_index(stmt, ":buid");
	int uuid = sqlite3_bind_parameter_index(stmt, ":uuid");
	int food = sqlite3_bind_parameter_index(stmt, ":food");
	int wood = sqlite3_bind_parameter_index(stmt, ":wood");
	int stone = sqlite3_bind_parameter_index(stmt, ":stone");
	int gold = sqlite3_bind_parameter_index(stmt, ":gold");

	for (auto p : players) {
		sqlite3_bind_int(stmt, id, p->getId());
		sqlite3_bind_int(stmt, act, p->isActive());
		sqlite3_bind_int(stmt, team, p->getTeam());
		sqlite3_bind_int(stmt, nat, p->getNation());
		sqlite3_bind_text(stmt, name, p->getName().c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, col, p->getColor());
		sqlite3_bind_int(stmt, buid, p->getBuildingUid());
		sqlite3_bind_int(stmt, uuid, p->getUnitUid());
		sqlite3_bind_int(stmt, food, p->getFood());
		sqlite3_bind_int(stmt, wood, p->getWood());
		sqlite3_bind_int(stmt, stone, p->getStone());
		sqlite3_bind_int(stmt, gold, p->getGold());

		rc = sqlite3_step(stmt);
		ifError(rc, nullptr, sql);

		sqlite3_reset(stmt);
		sqlite3_clear_bindings(stmt);
	}

	sqlite3_finalize(stmt);
}

void SceneSaver::saveConfig(int mapId, int size) {
	savingProgress.inc("saving config");
	createTable(SQLConsts::CONFIG_COL);
	const char* sql = "INSERT INTO config (precision, map, size) VALUES (:precision, :map, :size);";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr);
	ifError(rc, nullptr, sql);

	int prec = sqlite3_bind_parameter_index(stmt, ":precision");
	int map = sqlite3_bind_parameter_index(stmt, ":map");
	int sz = sqlite3_bind_parameter_index(stmt, ":size");

	sqlite3_bind_int(stmt, prec, precision);
	sqlite3_bind_int(stmt, map, mapId);
	sqlite3_bind_int(stmt, sz, size);

	rc = sqlite3_step(stmt);
	ifError(rc, nullptr, sql);

	sqlite3_finalize(stmt);
}

void SceneSaver::close() {
	sqlite3_close_v2(database);
	savingProgress.inc("");
}


