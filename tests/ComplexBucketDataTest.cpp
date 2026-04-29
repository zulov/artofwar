#include "pch.h"

#include "env/bucket/ComplexBucketData.h"
#include "env/bucket/ComplexBucketData.cpp"

class ComplexBucketDataFixture : public ::testing::Test {};

TEST_F(ComplexBucketDataFixture, NeighborOccupancyFlagsCanBeSetAndCleared) {
	ComplexBucketData data;

	EXPECT_TRUE(data.allNeightFree());
	EXPECT_FALSE(data.anyNeightOccupied());

	data.setNeightOccupied(0);
	data.setNeightOccupied(3);

	EXPECT_TRUE(data.anyNeightOccupied());
	EXPECT_TRUE(data.anyNeightFree());
	EXPECT_EQ(data.getNeightOccupation(), static_cast<unsigned char>(0x09));

	data.setNeightFree(0);
	EXPECT_EQ(data.getNeightOccupation(), static_cast<unsigned char>(0x08));

	data.setAllOccupied();
	EXPECT_TRUE(data.allNeightOccupied());
	EXPECT_FALSE(data.anyNeightFree());
}

TEST_F(ComplexBucketDataFixture, BuildableCollectableAndAttackableDependOnStateAndSize) {
	ComplexBucketData data;

	EXPECT_TRUE(data.isBuildable());
	EXPECT_TRUE(data.cellIsCollectable());
	EXPECT_TRUE(data.cellIsAttackable());

	data.incStateSize(CellState::COLLECT);
	EXPECT_EQ(data.getType(), CellState::COLLECT);
	EXPECT_TRUE(data.isBuildable());
	EXPECT_TRUE(data.cellIsCollectable());
	EXPECT_FALSE(data.cellIsAttackable());

	data.incStateSize(CellState::COLLECT);
	data.incStateSize(CellState::COLLECT);
	EXPECT_FALSE(data.cellIsCollectable());
	EXPECT_FALSE(data.cellIsAttackable());

	data.decStateSize();
	data.decStateSize();
	data.decStateSize();
	EXPECT_EQ(data.getType(), CellState::NONE);
	EXPECT_TRUE(data.cellIsCollectable());
	EXPECT_TRUE(data.cellIsAttackable());
}

TEST_F(ComplexBucketDataFixture, DeployStateIsPassableAndAttackableButNotBuildable) {
	ComplexBucketData data;
	data.setDeploy();

	EXPECT_EQ(data.getType(), CellState::DEPLOY);
	EXPECT_TRUE(data.isPassable());
	EXPECT_FALSE(data.isBuildable());
	EXPECT_TRUE(data.cellIsAttackable());
	EXPECT_FALSE(data.cellIsCollectable());
}

TEST_F(ComplexBucketDataFixture, GradientAndCloseIndexMetadataRoundTrip) {
	ComplexBucketData data;

	data.setGradient(12);
	data.setIndexCloseIndexes({3, 14});

	EXPECT_EQ(data.getGradient(), 12);
	EXPECT_EQ(data.getIndexOfCloseIndexes(), 3);
	EXPECT_EQ(data.getIndexSecondOfCloseIndexes(), 14);
}

TEST_F(ComplexBucketDataFixture, ResetForReuseRestoresDefaultState) {
	ComplexBucketData data;
	data.setNeightOccupied(2);
	data.setDeploy();
	data.setGradient(9);
	data.setIndexCloseIndexes({1, 2});
	data.setResBonuses(0, 0, 2.5f);

	data.resetForReuse();

	EXPECT_EQ(data.getType(), CellState::NONE);
	EXPECT_EQ(data.getAdditionalInfo(), -1);
	EXPECT_TRUE(data.allNeightFree());
	EXPECT_EQ(data.getGradient(), -1);
	EXPECT_EQ(data.getCost(), 0);
	EXPECT_FLOAT_EQ(data.getResBonus(0, 0), 1.f);
}

TEST_F(ComplexBucketDataFixture, ResourceBonusesDefaultToOneWhenUnset) {
	ComplexBucketData data;
	EXPECT_FLOAT_EQ(data.getResBonus(0, 0), 1.f);
	EXPECT_FLOAT_EQ(data.getResBonus(1, 3), 1.f);
}

TEST_F(ComplexBucketDataFixture, FirstResourceBonusInitializationShouldNotPolluteOtherEntries) {
	ComplexBucketData data;
	data.setResBonuses(0, 1, 2.5f);

	EXPECT_FLOAT_EQ(data.getResBonus(0, 1), 2.5f);
	EXPECT_FLOAT_EQ(data.getResBonus(0, 0), 1.f);
	EXPECT_FLOAT_EQ(data.getResBonus(0, 2), 1.f);
	EXPECT_FLOAT_EQ(data.getResBonus(1, 1), 1.f);
}

TEST_F(ComplexBucketDataFixture, ResourceBonusesShouldKeepMaximumPerEntry) {
	ComplexBucketData data;
	data.setResBonuses(1, 2, 1.5f);
	data.setResBonuses(1, 2, 3.5f);
	data.setResBonuses(1, 2, 2.0f);

	EXPECT_FLOAT_EQ(data.getResBonus(1, 2), 3.5f);
}

TEST_F(ComplexBucketDataFixture, ResetResBonusesRemovesStoredBonuses) {
	ComplexBucketData data;
	data.setResBonuses(0, 0, 4.f);

	data.resetResBonuses();

	EXPECT_FLOAT_EQ(data.getResBonus(0, 0), 1.f);
}
