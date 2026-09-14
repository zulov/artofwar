#include "pch.h"

#include "math/RandGen.h"
#include "player/Resources.h"
#include "simulation/FrameInfo.h"

TEST(PersistenceStateTest, RandomStateContinuesBothSequences) {
	RandGen::init(false);
	RandGen::nextRand(RandFloatType::AI);
	RandGen::nextRand(RandIntType::SAVE);
	const auto state = RandGen::saveState();
	const auto expectedFloat = RandGen::nextRand(RandFloatType::AI);
	const auto expectedInt = RandGen::nextRand(RandIntType::SAVE, 1000000);

	RandGen::reset(false);
	RandGen::loadState(state);

	EXPECT_FLOAT_EQ(expectedFloat, RandGen::nextRand(RandFloatType::AI));
	EXPECT_EQ(expectedInt, RandGen::nextRand(RandIntType::SAVE, 1000000));
}

TEST(PersistenceStateTest, RandomStateRestoresGeneratedSequenceFromSeed) {
	RandGen::init(true);
	RandGen::nextRand(RandFloatType::AI);
	RandGen::nextRand(RandIntType::SAVE);
	const auto state = RandGen::saveState();
	const auto expectedFloat = RandGen::nextRand(RandFloatType::AI);
	const auto expectedInt = RandGen::nextRand(RandIntType::SAVE, 1000000);

	RandGen::init(true);
	RandGen::loadState(state);

	EXPECT_FLOAT_EQ(expectedFloat, RandGen::nextRand(RandFloatType::AI));
	EXPECT_EQ(expectedInt, RandGen::nextRand(RandIntType::SAVE, 1000000));
}

TEST(PersistenceStateTest, FrameStateRestoresTicksAndResetsTransientTime) {
	FrameInfo original;
	original.accumulate(TIME_PER_UPDATE * 2.f);
	original.countFrame();
	original.addWallTime(1.25f);

	FrameInfo restored;
	restored.loadState(original.saveState());

	EXPECT_EQ(original.getCurrentFrame(), restored.getCurrentFrame());
	EXPECT_EQ(original.getSeconds(), restored.getSeconds());
	EXPECT_EQ(original.getTotalTicks(), restored.getTotalTicks());
	EXPECT_FLOAT_EQ(0.f, restored.getWallTime());
	EXPECT_FLOAT_EQ(0.f, restored.getAccumulateTime());
}

TEST(PersistenceStateTest, FrameStateDerivesFrameAndSecondsFromTicks) {
	FrameInfo info;
	info.set(7, 125);

	EXPECT_EQ(125u * FRAMES_IN_PERIOD + 7u, info.getTotalTicks());
	EXPECT_EQ(7, info.getCurrentFrame());
	EXPECT_EQ(125u, info.getSeconds());

	const auto state = info.saveState();
	EXPECT_EQ(125u * FRAMES_IN_PERIOD + 7u, state.totalTicks);
}

TEST(PersistenceStateTest, ResourceRuntimeStateRestoresEconomyInputs) {
	ResourcesSaveData state;
	state.gatherSpeeds1s = {4.f, 8.f, 10.f, 6.f};
	state.sumGatherSpeed = {1.f, 2.f, 3.f, 4.f};
	state.sumValues = {10.f, 20.f, 30.f, 40.f};

	Resources resources;
	resources.setValue(100.f, 0.f, 0.f, 100.f);
	resources.loadState(state);

	EXPECT_FLOAT_EQ(4.f, resources.getGatherSpeed(ResourceType::FOOD));
	EXPECT_FLOAT_EQ(10.f, resources.getGatherSpeed(ResourceType::STONE));
	EXPECT_EQ(0, resources.getFoodStorage());
	EXPECT_FLOAT_EQ(0.f, resources.getStoneRefineCapacity());
	EXPECT_FLOAT_EQ(0.f, resources.getGoldStorage());
	EXPECT_FLOAT_EQ(0.f, resources.getGoldRefineCapacity());
	EXPECT_FLOAT_EQ(10.f, resources.potentialFoodLost());
	EXPECT_FLOAT_EQ(0.f, resources.potentialGoldGain());
	EXPECT_FLOAT_EQ(0.f, resources.getPotentialStoneRefinement());
	EXPECT_EQ(30.f, resources.getSumValues()[2]);
}
