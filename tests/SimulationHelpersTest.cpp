#include "pch.h"

#include <tuple>
#include <utility>

#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/Str.h>

#include "Progress.h"
#include "env/path/PathCache.h"
#include "simulation/FrameInfo.h"

class SimulationHelpersFixture : public ::testing::Test {};

TEST_F(SimulationHelpersFixture, FrameInfoSetStoresValuesAndMarksRealFrame) {
	FrameInfo info;
	info.set(7, 125);

	EXPECT_EQ(info.getCurrentFrame(), 7);
	EXPECT_EQ(info.getSeconds(), 125u);
	EXPECT_TRUE(info.isRealFrame());
}

TEST_F(SimulationHelpersFixture, FrameInfoResetRealFrameClearsOnlyFlag) {
	FrameInfo info;
	info.set(3, 10);
	info.resetRealFrame();

	EXPECT_EQ(info.getCurrentFrame(), 3);
	EXPECT_EQ(info.getSeconds(), 10u);
	EXPECT_FALSE(info.isRealFrame());
}

TEST_F(SimulationHelpersFixture, FrameInfoDateRollsMonthsIntoYears) {
	FrameInfo info;
	info.set(0, SECONDS_IN_MONTH * (12 * 3 + 5));
	auto [month, year] = info.getDate();

	EXPECT_EQ(month, 5);
	EXPECT_EQ(year, 3);
}

TEST_F(SimulationHelpersFixture, FrameInfoTimeSplitsIntoHoursMinutesAndSeconds) {
	FrameInfo info;
	info.set(0, 3661);
	auto [hours, minutes, seconds] = info.getTime();

	EXPECT_EQ(hours, 1);
	EXPECT_EQ(minutes, 1);
	EXPECT_EQ(seconds, 1);
}

TEST_F(SimulationHelpersFixture, PerFrameActionQueueHudTriggersEveryTwoSecondsOnFirstFrame) {
	EXPECT_FALSE(FrameInfo::get(PerFrameAction::QUEUE_HUD, 0, 0));
	EXPECT_TRUE(FrameInfo::get(PerFrameAction::QUEUE_HUD, 0, 1));
	EXPECT_FALSE(FrameInfo::get(PerFrameAction::QUEUE_HUD, 1, 1));
}

TEST_F(SimulationHelpersFixture, PerFrameActionMonthlyAndYearlyUpdatesUseLastFrame) {
	EXPECT_FALSE(FrameInfo::get(PerFrameAction::RESOURCE_MONTH_UPDATE, 29, 3));
	EXPECT_TRUE(FrameInfo::get(PerFrameAction::RESOURCE_MONTH_UPDATE, 29, 4));
	EXPECT_FALSE(FrameInfo::get(PerFrameAction::RESOURCE_MONTH_UPDATE, 28, 4));

	EXPECT_FALSE(FrameInfo::get(PerFrameAction::RESOURCE_YEAR_UPDATE, 29, 58));
	EXPECT_TRUE(FrameInfo::get(PerFrameAction::RESOURCE_YEAR_UPDATE, 29, 59));
	EXPECT_FALSE(FrameInfo::get(PerFrameAction::RESOURCE_YEAR_UPDATE, 28, 59));
}

TEST_F(SimulationHelpersFixture, PerFrameActionFrameInfoOverloadMatchesDirectLookup) {
	FrameInfo info;
	info.set(29, 59);

	EXPECT_EQ(
		info.shouldRun(PerFrameAction::RESOURCE_YEAR_UPDATE),
		FrameInfo::get(PerFrameAction::RESOURCE_YEAR_UPDATE, 29, 59)
	);
}

TEST_F(SimulationHelpersFixture, PerFrameActionSpecificFramesMatchConfiguredSchedules) {
	EXPECT_TRUE(FrameInfo::get(PerFrameAction::INFLUENCE_UNITS, 0, 1));
	EXPECT_FALSE(FrameInfo::get(PerFrameAction::INFLUENCE_UNITS, 1, 1));
	EXPECT_FALSE(FrameInfo::get(PerFrameAction::INFLUENCE_OTHER, 3, 3));
	EXPECT_TRUE(FrameInfo::get(PerFrameAction::INFLUENCE_OTHER, 3, 4));
	EXPECT_TRUE(FrameInfo::get(PerFrameAction::INFLUENCE_HISTORY_RESET, 3, 29));
	EXPECT_TRUE(FrameInfo::get(PerFrameAction::VISIBILITY, 6, 0));
	EXPECT_TRUE(FrameInfo::get(PerFrameAction::AI_ORDER, 14, 0));
	EXPECT_TRUE(FrameInfo::get(PerFrameAction::SELF_AI, 11, 0));
	EXPECT_TRUE(FrameInfo::get(PerFrameAction::SELF_AI, 29, 0));
}

TEST_F(SimulationHelpersFixture, PathCacheSetCopiesPathAndMatchesEndpoints) {
	PathCache cache;
	std::vector<int> source = { 4, 5, 6 };

	cache.set(10, 20, &source);
	source[0] = 99;

	EXPECT_TRUE(cache.equal(10, 20));
	EXPECT_EQ(cache.path, std::vector<int>({ 4, 5, 6 }));
}

TEST_F(SimulationHelpersFixture, PathCacheClearResetsStoredState) {
	PathCache cache;
	std::vector<int> source = { 1, 2 };
	cache.set(7, 9, &source);

	cache.clear();

	EXPECT_EQ(cache.start, -1);
	EXPECT_EQ(cache.end, -1);
	EXPECT_TRUE(cache.path.empty());
	EXPECT_FALSE(cache.equal(7, 9));
}

TEST_F(SimulationHelpersFixture, ProgressStartsAtZeroAndAdvancesWithInc) {
	Progress progress(4, false);

	EXPECT_EQ(static_cast<int>(progress.currentStage), 0);
	EXPECT_FLOAT_EQ(progress.getProgress(), 0.f);

	progress.inc();
	EXPECT_EQ(static_cast<int>(progress.currentStage), 1);
	EXPECT_FLOAT_EQ(progress.getProgress(), 0.25f);

	progress.inc("ignored");
	EXPECT_EQ(static_cast<int>(progress.currentStage), 2);
	EXPECT_FLOAT_EQ(progress.getProgress(), 0.5f);
}

TEST_F(SimulationHelpersFixture, ProgressResetRestoresInitialState) {
	Progress progress(3, false);
	progress.inc();
	progress.inc();

	progress.reset();

	EXPECT_EQ(static_cast<int>(progress.currentStage), 0);
	EXPECT_FLOAT_EQ(progress.getProgress(), 0.f);
}
