#include "pch.h"

#include <sqlite3/sqlite3.h>

#include "database/db_other_struct.h"
#include "database/db_world_age_struct.h"
#include "simulation/WorldAgeController.h"
#include "simulation/WorldAgeController.cpp"

int Player::getWorkersNumber() const {
	return 0;
}

unsigned Possession::getArmyNumber() {
	return 0;
}

namespace {
struct SqliteStatement {
	sqlite3* database{};
	sqlite3_stmt* statement{};

	SqliteStatement(const char* sql) {
		EXPECT_EQ(SQLITE_OK, sqlite3_open(":memory:", &database));
		EXPECT_EQ(SQLITE_OK, sqlite3_prepare_v2(database, sql, -1, &statement, nullptr));
		EXPECT_EQ(SQLITE_ROW, sqlite3_step(statement));
	}

	~SqliteStatement() {
		sqlite3_finalize(statement);
		sqlite3_close(database);
	}
};
}

TEST(WorldAgeControllerTest, ReturnsEveryNextStageCandidateInMapOrder) {
	SqliteStatement age0Sql("SELECT 0, 0, 'Settlement';");
	SqliteStatement age1Sql("SELECT 1, 1, 'Growth';");
	SqliteStatement age2Sql("SELECT 2, 1, 'Mobilization';");
	SqliteStatement mapSql("SELECT 0, 'map.xml', 'Map', '0,2,1';");

	auto* age0 = new db_world_age(age0Sql.statement);
	auto* age1 = new db_world_age(age1Sql.statement);
	auto* age2 = new db_world_age(age2Sql.statement);
	db_world_age_catalog catalog;
	catalog.ages = {age0, age1, age2};
	const db_map map(mapSql.statement);
	WorldAgeController controller(&catalog, &map);

	const std::vector<Player*> players;
	const auto progress = controller.getNextAgeProgress(players);

	ASSERT_EQ(2u, progress.size());
	EXPECT_EQ(2, progress[0].ageId);
	EXPECT_EQ(1, progress[1].ageId);
	EXPECT_FLOAT_EQ(0.f, progress[0].progress);
	EXPECT_FLOAT_EQ(0.f, progress[1].progress);
}

TEST(WorldAgeControllerTest, ReturnsNoCandidatesWhenTheCatalogHasNoNextStage) {
	SqliteStatement ageSql("SELECT 0, 0, 'Settlement';");
	SqliteStatement mapSql("SELECT 0, 'map.xml', 'Map', '0';");

	db_world_age_catalog catalog;
	catalog.ages.push_back(new db_world_age(ageSql.statement));
	const db_map map(mapSql.statement);
	WorldAgeController controller(&catalog, &map);

	EXPECT_TRUE(controller.getNextAgeProgress({}).empty());
}
