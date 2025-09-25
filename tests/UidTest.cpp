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

	EXPECT_EQ(2952790016, uidBuilding.getId());
	EXPECT_EQ(ObjectType::BUILDING, uidBuilding.getType());
	EXPECT_EQ(5, uidBuilding.getPlayer());

	EXPECT_EQ(2952790017, uidBuildingInc.getId());
	EXPECT_EQ(ObjectType::BUILDING, uidBuildingInc.getType());
	EXPECT_EQ(5, uidBuildingInc.getPlayer());

	EXPECT_EQ(671088640, uidUnit.getId());
	EXPECT_EQ(ObjectType::UNIT, uidUnit.getType());
	EXPECT_EQ(1, uidUnit.getPlayer());

	EXPECT_EQ(0, uidResource.getId());
	EXPECT_EQ(ObjectType::RESOURCE, uidResource.getType());
	EXPECT_EQ(-1, uidResource.getPlayer());
}
