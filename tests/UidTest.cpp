#include "pch.h"

#include "objects/UId.h"

class UidFixture : public ::testing::Test {
public:

protected:
	static void SetUpTestCase() {
	}

	static void TearDownTestCase() {
	}
};


TEST_F(UidFixture, Create) {
	auto uidBuilding = UId(UId::create(ObjectType::BUILDING, 5));

	auto uidUnit = UId(UId::create(ObjectType::UNIT, 1));
	auto uidResource = UId(UId::create(ObjectType::RESOURCE));

	auto uidBuildingInc = UId(uidBuilding.getId() + 1);

	EXPECT_EQ(2952790016, uidBuilding.getId()); // type=BUILDING(0xB0), player=5 -> 0xB0280000
	EXPECT_EQ(ObjectType::BUILDING, uidBuilding.getType());
	EXPECT_EQ(5, uidBuilding.getPlayer());

	EXPECT_EQ(2952790017, uidBuildingInc.getId()); // same prefix, counter +1
	EXPECT_EQ(ObjectType::BUILDING, uidBuildingInc.getType());
	EXPECT_EQ(5, uidBuildingInc.getPlayer());

	EXPECT_EQ(671088640, uidUnit.getId()); // type=UNIT(0x28), player=1 -> 0x28040000
	EXPECT_EQ(ObjectType::UNIT, uidUnit.getType());
	EXPECT_EQ(1, uidUnit.getPlayer());

	EXPECT_EQ(0, uidResource.getId()); // type=RESOURCE(0x00), no player
	EXPECT_EQ(ObjectType::RESOURCE, uidResource.getType());
	EXPECT_EQ(-1, uidResource.getPlayer());
}

TEST_F(UidFixture, DifferentPlayersSameType) {
	auto uid0 = UId(UId::create(ObjectType::UNIT, 0));
	auto uid1 = UId(UId::create(ObjectType::UNIT, 1));

	EXPECT_EQ(ObjectType::UNIT, uid0.getType());
	EXPECT_EQ(ObjectType::UNIT, uid1.getType());
	EXPECT_EQ(0, uid0.getPlayer());
	EXPECT_EQ(1, uid1.getPlayer());
	EXPECT_NE(uid0.getId(), uid1.getId());
}

TEST_F(UidFixture, DifferentTypesPreserved) {
	auto uidBuilding = UId(UId::create(ObjectType::BUILDING, 0));
	auto uidUnit = UId(UId::create(ObjectType::UNIT, 0));
	auto uidResource = UId(UId::create(ObjectType::RESOURCE));

	EXPECT_NE(uidBuilding.getType(), uidUnit.getType());
	EXPECT_NE(uidUnit.getType(), uidResource.getType());
	EXPECT_NE(uidBuilding.getType(), uidResource.getType());
}

TEST_F(UidFixture, IncrementPreservesTypeAndPlayer) {
	auto base = UId(UId::create(ObjectType::UNIT, 1));
	for (unsigned int i = 1; i <= 10; ++i) {
		auto incremented = UId(base.getId() + i);
		EXPECT_EQ(ObjectType::UNIT, incremented.getType());
		EXPECT_EQ(1, incremented.getPlayer());
	}
}
