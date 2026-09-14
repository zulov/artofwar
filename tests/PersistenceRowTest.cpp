#include "pch.h"

#include <array>

#include "database/db_insert_defs.h"
#include "database/db_read_defs.h"
#include "player/ai/WantList.h"
#include "scene/save/SaveTable.h"

namespace {
	template <typename Col>
	void checkSaveTableContract(sqlite3* database) {
		const auto createSql = std::string("CREATE TABLE ") + SaveTable<Col>::name + SaveTable<Col>::schema;
		ASSERT_TRUE(execSql(database, createSql.c_str()));
		sqlite3_stmt* statement = nullptr;
		ASSERT_EQ(SQLITE_OK, sqlite3_prepare_v2(database, saveSelectSql<Col>().c_str(), -1, &statement, nullptr));
		sqlite3_finalize(statement);
	}

	sqlite3* makeDatabase(const char* name, const char* schema) {
		sqlite3* database = nullptr;
		if (sqlite3_open(":memory:", &database) != SQLITE_OK) {
			return database;
		}
		EXPECT_TRUE(execSql(database, (std::string("CREATE TABLE ") + name + schema).c_str()));
		return database;
	}

	template <typename Col, typename Row>
	Row roundTrip(const Row& expected) {
		sqlite3* database = makeDatabase(SaveTable<Col>::name, SaveTable<Col>::schema);
		if (!database) {
			return {};
		}
		const auto sql = make_insert_sql(SaveTable<Col>::name, saveColumns<Col>());
		const bool inserted = executeBatch(database, sql.c_str(), [&](sqlite3_stmt* stmt, const char* insertSql) {
			bindRow(stmt, 1, &expected);
			return stepAndReset(stmt, insertSql);
		});
		EXPECT_TRUE(inserted);

		Row actual{};
		if (inserted) {
			loadFromTable(database, saveSelectSql<Col>(), [&](sqlite3_stmt* stmt) { actual = readRow<Row>(stmt, 1); });
		}
		sqlite3_close(database);
		return actual;
	}
}

TEST(PersistenceRowTest, AllSaveTableContractsCreateAndSelect) {
	sqlite3* database = nullptr;
	ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &database));
	ASSERT_TRUE(database);

	checkSaveTableContract<UnitCol>(database);
	checkSaveTableContract<BuildingCol>(database);
	checkSaveTableContract<ResourceCol>(database);
	checkSaveTableContract<PlayerCol>(database);
	checkSaveTableContract<ConfigCol>(database);
	checkSaveTableContract<UnitOrderCol>(database);
	checkSaveTableContract<AimPathCol>(database);
	checkSaveTableContract<QueueCol>(database);
	checkSaveTableContract<PlayerLevelCol>(database);
	checkSaveTableContract<AiStateCol>(database);
	checkSaveTableContract<AiWantCol>(database);
	checkSaveTableContract<AiHistoryCol>(database);
	checkSaveTableContract<ProjectileCol>(database);
	checkSaveTableContract<FormationCol>(database);
	checkSaveTableContract<FormationOrderCol>(database);
	checkSaveTableContract<PendingCommandCol>(database);
	checkSaveTableContract<PendingCommandEntityCol>(database);

	sqlite3_close(database);
}

TEST(PersistenceRowTest, AiStateRoundTripsAllPressureValues) {
	AiSaveData expected;
	expected.player = 3;
	expected.prevScore = 120;
	expected.prevEnemyScore = 95;
	expected.prevUnits = 17;
	expected.prevResSum = 44.5f;
	expected.prevGatherSum = 12.25f;
	expected.foodPriority = 0.1f;
	expected.woodPriority = 0.2f;
	expected.stonePriority = 0.3f;
	expected.goldPriority = 0.4f;
	expected.lackingPerResource = {1.f, 2.f, 3.f, 4.f};
	for (size_t i = 0; i < expected.militaryPressure.size(); ++i) {
		expected.militaryPressure[i] = static_cast<float>(i) - 10.f;
	}

	const auto actual = roundTrip<AiStateCol>(expected);
	EXPECT_EQ(expected.player, actual.player);
	EXPECT_EQ(expected.prevScore, actual.prevScore);
	EXPECT_EQ(expected.prevEnemyScore, actual.prevEnemyScore);
	EXPECT_EQ(expected.prevUnits, actual.prevUnits);
	EXPECT_FLOAT_EQ(expected.prevResSum, actual.prevResSum);
	EXPECT_FLOAT_EQ(expected.prevGatherSum, actual.prevGatherSum);
	EXPECT_EQ(expected.lackingPerResource, actual.lackingPerResource);
	for (size_t i = 0; i < expected.militaryPressure.size(); ++i) {
		EXPECT_FLOAT_EQ(expected.militaryPressure[i], actual.militaryPressure[i]);
	}
}

TEST(PersistenceRowTest, AiWantRoundTripsSchedulerState) {
	const AiWantRow expected{{2, 4.5f, 3.f, static_cast<unsigned char>(WantItemType::UNIT), 2, 7, 9, 12, true}, 1};
	const auto actual = roundTrip<AiWantCol>(expected);

	EXPECT_EQ(expected.data.player, actual.data.player);
	EXPECT_FLOAT_EQ(expected.data.priority, actual.data.priority);
	EXPECT_FLOAT_EQ(expected.data.basePriority, actual.data.basePriority);
	EXPECT_EQ(expected.data.type, actual.data.type);
	EXPECT_EQ(expected.data.count, actual.data.count);
	EXPECT_EQ(expected.data.specificId, actual.data.specificId);
	EXPECT_EQ(expected.data.age, actual.data.age);
	EXPECT_EQ(expected.data.reserveTicks, actual.data.reserveTicks);
	EXPECT_EQ(expected.data.active, actual.data.active);
	EXPECT_EQ(expected.orderIndex, actual.orderIndex);
}

TEST(PersistenceRowTest, FormationRowsRoundTripContinuationFields) {
	const FormationSaveData expected{3, 1, 1, -0.5f, 0.75f};
	const auto actual = roundTrip<FormationCol>(expected);

	EXPECT_EQ(expected.id, actual.id);
	EXPECT_EQ(expected.state, actual.state);
	EXPECT_EQ(expected.type, actual.type);
	EXPECT_FLOAT_EQ(expected.directionX, actual.directionX);
	EXPECT_FLOAT_EQ(expected.directionZ, actual.directionZ);
}

TEST(PersistenceRowTest, ProjectileRowsRoundTripLogicalState) {
	const ProjectileSaveData expected{2201, 0.45f, 1.2f, 8.f, 1};
	const auto actual = roundTrip<ProjectileCol>(expected);

	EXPECT_EQ(expected.aimUid, actual.aimUid);
	EXPECT_FLOAT_EQ(expected.percentToGo, actual.percentToGo);
	EXPECT_FLOAT_EQ(expected.speed, actual.speed);
	EXPECT_FLOAT_EQ(expected.attackVal, actual.attackVal);
	EXPECT_EQ(expected.player, actual.player);
}

TEST(PersistenceRowTest, ConfigRoundTripsGlobalContinuationState) {
	ConfigSaveData expected;
	expected.precision = 10;
	expected.map = 2;
	expected.size = 128;
	expected.totalTicks = 1907;
	expected.randomPresent = true;
	expected.random = {1234, {1, 2, 3, 4}, {5, 6, 7}};

	const auto actual = roundTrip<ConfigCol>(expected);

	EXPECT_EQ(expected.precision, actual.precision);
	EXPECT_EQ(expected.map, actual.map);
	EXPECT_EQ(expected.size, actual.size);
	EXPECT_EQ(expected.totalTicks, actual.totalTicks);
	EXPECT_EQ(expected.randomPresent, actual.randomPresent);
	EXPECT_EQ(expected.random.seed, actual.random.seed);
	EXPECT_EQ(expected.random.floatIndexes, actual.random.floatIndexes);
	EXPECT_EQ(expected.random.intIndexes, actual.random.intIndexes);
}

TEST(PersistenceRowTest, PendingCommandRoundTripsScalarFields) {
	PendingCommandSaveData expected;
	expected.order = 4;
	expected.kind = PendingCommandKind::GROUP_ORDER;
	expected.action = 2;
	expected.actionType = 3;
	expected.id = 7;
	expected.player = 1;
	expected.level = 2;
	expected.number = 9;
	expected.x = 14.5f;
	expected.z = -3.25f;
	expected.hp = 80.f;
	expected.targetUid = 1001;
	expected.formationId = 6;
	expected.append = true;
	expected.entityUids = {10, 11};

	const auto actual = roundTrip<PendingCommandCol>(expected);
	EXPECT_EQ(expected.order, actual.order);
	EXPECT_EQ(expected.kind, actual.kind);
	EXPECT_EQ(expected.action, actual.action);
	EXPECT_EQ(expected.actionType, actual.actionType);
	EXPECT_EQ(expected.id, actual.id);
	EXPECT_EQ(expected.player, actual.player);
	EXPECT_EQ(expected.level, actual.level);
	EXPECT_EQ(expected.number, actual.number);
	EXPECT_FLOAT_EQ(expected.x, actual.x);
	EXPECT_FLOAT_EQ(expected.z, actual.z);
	EXPECT_FLOAT_EQ(expected.hp, actual.hp);
	EXPECT_EQ(expected.targetUid, actual.targetUid);
	EXPECT_EQ(expected.formationId, actual.formationId);
	EXPECT_EQ(expected.append, actual.append);
	EXPECT_TRUE(actual.entityUids.empty());
}

TEST(PersistenceRowTest, GeneratedOrderIndexIsStoredInTheRow) {
	const UnitOrderRow expected{{42, 5, true, false, 0, 8.f, 9.f}, 3};
	const auto actual = roundTrip<UnitOrderCol>(expected);

	EXPECT_EQ(expected.data.unitUid, actual.data.unitUid);
	EXPECT_EQ(expected.data.action, actual.data.action);
	EXPECT_EQ(expected.orderIndex, actual.orderIndex);
	EXPECT_FLOAT_EQ(expected.data.x, actual.data.x);
	EXPECT_FLOAT_EQ(expected.data.z, actual.data.z);
}

TEST(PersistenceRowTest, ReportsMissingSaveColumnsWithTheQuery) {
	sqlite3* database = nullptr;
	ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &database));
	ASSERT_TRUE(database);
	ASSERT_TRUE(execSql(database, "CREATE TABLE config (precision INT NOT NULL, map INT NOT NULL, size INT NOT NULL);"));

	std::string error;
	const bool loaded = loadFromTable(database, saveSelectSql<ConfigCol>(), [](sqlite3_stmt*) {}, &error);

	EXPECT_FALSE(loaded);
	EXPECT_NE(std::string::npos, error.find("prepare failed"));
	EXPECT_NE(std::string::npos, error.find("total_ticks"));
	EXPECT_NE(std::string::npos,
			error.find("SELECT precision, map, size, total_ticks, random_present, random_seed, "
				"random_float_ai_index"));
	sqlite3_close(database);
}
