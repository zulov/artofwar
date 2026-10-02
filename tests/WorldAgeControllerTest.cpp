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
	age0->nextAgeIds = {2, 1};
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

TEST(WorldAgeControllerTest, ReturnsOnlyCandidatesListedBySelectedMap) {
	SqliteStatement age0Sql("SELECT 0, 0, 'age_0';");
	SqliteStatement age1Sql("SELECT 1, 1, 'age_1';");
	SqliteStatement age2Sql("SELECT 2, 1, 'age_2';");
	SqliteStatement mapSql("SELECT 0, 'map.xml', 'Map', '0,1';");

	db_world_age_catalog catalog;
	auto* age0 = new db_world_age(age0Sql.statement);
	auto* age1 = new db_world_age(age1Sql.statement);
	auto* age2 = new db_world_age(age2Sql.statement);
	age0->nextAgeIds = {1};
	catalog.ages = {age0, age1, age2};
	const db_map map(mapSql.statement);
	WorldAgeController controller(&catalog, &map);

	const auto progress = controller.getNextAgeProgress({});

	ASSERT_EQ(1u, progress.size());
	EXPECT_EQ(1, progress.front().ageId);
}

TEST(WorldAgeControllerTest, FollowsSelectedBranchInsteadOfShowingEveryAgeAtTheNextStage) {
	std::vector<SqliteStatement> ageSql;
	ageSql.reserve(7);
	for (int ageId = 0; ageId < 7; ++ageId) {
		ageSql.emplace_back(("SELECT " + std::to_string(ageId) + ", " + std::to_string(ageId == 0 ? 0 : (ageId + 1) / 2)
										 + ", 'age_" + std::to_string(ageId) + "';").c_str());
	}

	db_world_age_catalog catalog;
	for (auto& statement : ageSql) {
		catalog.ages.push_back(new db_world_age(statement.statement));
	}
	catalog.ages[0]->nextAgeIds = {1, 2};
	catalog.ages[1]->nextAgeIds = {3};
	catalog.ages[2]->nextAgeIds = {4};
	catalog.ages[3]->nextAgeIds = {5};
	catalog.ages[4]->nextAgeIds = {6};
	SqliteStatement mapSql("SELECT 0, 'map.xml', 'Map', '0,1,2,3,4,5,6';");
	const db_map map(mapSql.statement);
	WorldAgeController controller(&catalog, &map);
	const std::vector<Player*> players = {nullptr};

	controller.update(players, 1);
	ASSERT_EQ(1, controller.getCurrentAgeId());
	const auto afterFirstBranch = controller.getNextAgeProgress(players);
	ASSERT_EQ(1u, afterFirstBranch.size());
	EXPECT_EQ(3, afterFirstBranch.front().ageId);

	controller.update(players, 2);
	ASSERT_EQ(3, controller.getCurrentAgeId());
	const auto afterSecondAge = controller.getNextAgeProgress(players);
	ASSERT_EQ(1u, afterSecondAge.size());
	EXPECT_EQ(5, afterSecondAge.front().ageId);
}

TEST(WorldAgeControllerTest, ReportsAgeTimeoutProgress) {
	SqliteStatement ageSql("SELECT 0, 0, 'age_0';");
	SqliteStatement nextAgeSql("SELECT 1, 1, 'age_1';");
	SqliteStatement mapSql("SELECT 0, 'map.xml', 'Map', '0,1';");

	db_world_age_catalog catalog;
	catalog.ages = {new db_world_age(ageSql.statement), new db_world_age(nextAgeSql.statement)};
	const db_map map(mapSql.statement);
	WorldAgeController controller(&catalog, &map);
	controller.reset(100);

	EXPECT_FLOAT_EQ(0.f, controller.getTimeoutProgress(100));
	EXPECT_FLOAT_EQ(0.5f, controller.getTimeoutProgress(100 + WorldAgeController::AGE_TIMEOUT_TICKS / 2));
	EXPECT_FLOAT_EQ(1.f, controller.getTimeoutProgress(100 + WorldAgeController::AGE_TIMEOUT_TICKS));
	EXPECT_FLOAT_EQ(1.f, controller.getTimeoutProgress(100 + WorldAgeController::AGE_TIMEOUT_TICKS + 1));
}
