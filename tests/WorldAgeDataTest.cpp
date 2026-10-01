#include "pch.h"

#include "database/db_other_struct.h"
#include "database/db_world_age_struct.h"

TEST(WorldAgeDataTest, AgeReadsStageAndNameInMapTableOrder) {
	sqlite3* database = nullptr;
	sqlite3_stmt* statement = nullptr;
	ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &database));
	ASSERT_EQ(SQLITE_OK, sqlite3_prepare_v2(database, "SELECT 3, 2, 'Age of Consolidation';", -1, &statement, nullptr));
	ASSERT_EQ(SQLITE_ROW, sqlite3_step(statement));
	const db_world_age age(statement);

	EXPECT_EQ(3, age.id);
	EXPECT_EQ(2, age.stage);
	EXPECT_STREQ("Age of Consolidation", age.name.CString());
	sqlite3_finalize(statement);
	sqlite3_close(database);
}

TEST(WorldAgeDataTest, ConditionReadsMetricAndTargetInMapTableOrder) {
	sqlite3* database = nullptr;
	sqlite3_stmt* statement = nullptr;
	ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &database));
	ASSERT_EQ(SQLITE_OK, sqlite3_prepare_v2(database, "SELECT 4, 1, 50.0;", -1, &statement, nullptr));
	ASSERT_EQ(SQLITE_ROW, sqlite3_step(statement));
	const db_world_age_condition condition(statement);

	EXPECT_EQ(4, condition.id);
	EXPECT_EQ(WorldAgeMetric::ARMY_COUNT, condition.metric);
	EXPECT_FLOAT_EQ(50.f, condition.target);
	sqlite3_finalize(statement);
	sqlite3_close(database);
}

TEST(WorldAgeDataTest, AgeConditionJoinReadsAgeAndConditionIds) {
	sqlite3* database = nullptr;
	sqlite3_stmt* statement = nullptr;
	ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &database));
	ASSERT_EQ(SQLITE_OK, sqlite3_prepare_v2(database, "SELECT 3, 4;", -1, &statement, nullptr));
	ASSERT_EQ(SQLITE_ROW, sqlite3_step(statement));

	EXPECT_EQ(3, asUShort(statement, DbWorldAgeJoinCol::age_id));
	EXPECT_EQ(4, asUShort(statement, DbWorldAgeJoinCol::condition_id));
	sqlite3_finalize(statement);
	sqlite3_close(database);
}

TEST(WorldAgeDataTest, MapReadsOrderedAgeList) {
	sqlite3* database = nullptr;
	sqlite3_stmt* statement = nullptr;
	ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &database));
	ASSERT_EQ(SQLITE_OK,
			sqlite3_prepare_v2(database, "SELECT 1, 'map/map1.xml', 'Standard', '0,1,2,3,4,5,6';", -1, &statement, nullptr));
	ASSERT_EQ(SQLITE_ROW, sqlite3_step(statement));
	const db_map map(statement);

	ASSERT_EQ(7u, map.ageIds.size());
	EXPECT_EQ(0, map.ageIds.front());
	EXPECT_EQ(6, map.ageIds.back());
	EXPECT_STREQ("map/map1.xml", map.xmlName.CString());
	sqlite3_finalize(statement);
	sqlite3_close(database);
}
