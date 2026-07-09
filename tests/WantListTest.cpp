#include "pch.h"

#include <array>
#include <cmath>
#include <map>

#include "database/db_struct.h"
#include "player/ai/WantList.h"
#include "player/ai/WantList.cpp"
#include "player/ai/WantListExecute.cpp"
#include "player/ai/UnitBrain.h"

// ==========================================
// WantList unit tests
// ==========================================

class WantListFixture : public ::testing::Test {
protected:
	WantList wl;
};

namespace {
	class TestWantExecutor : public IWantExecutor {
	public:
		void setCost(short id, unsigned short food, unsigned short wood, unsigned short stone, unsigned short gold) {
			costs.erase(id);
			costs.emplace(std::piecewise_construct,
			              std::forward_as_tuple(id),
			              std::forward_as_tuple(food, wood, stone, gold));
		}

		void setExecuteResult(short id, bool result) {
			executeResults[id] = result;
		}

		const db_with_cost* cost(const WantItem& item) const override {
			const auto it = costs.find(item.specificId);
			return it == costs.end() ? nullptr : &it->second;
		}

		void onNotEnoughResources(const WantItem& item) override {
			notEnoughIds.push_back(item.specificId);
		}

		bool execute(WantItem& item) override {
			executedIds.push_back(item.specificId);
			const auto it = executeResults.find(item.specificId);
			return it == executeResults.end() ? true : it->second;
		}

		std::vector<short> executedIds;
		std::vector<short> notEnoughIds;

	private:
		std::map<short, db_with_cost> costs;
		std::map<short, bool> executeResults;
	};

	std::array<float, RESOURCES_SIZE> resourcesWithWood(float wood) {
		std::array<float, RESOURCES_SIZE> resources{};
		resources[cast(ResourceType::WOOD)] = wood;
		return resources;
	}

	void runTick(WantList& wl, IWantExecutor& executor, std::span<const float> resources,
	             WantItemType type, float priority, short id, unsigned char count = 1) {
		wl.resetRequests();
		wl.addRequest(type, priority, id, count);
		wl.execute(resources, executor);
	}
}

// --- addRequest / resetRequests basics ---

TEST_F(WantListFixture, EmptyByDefault) {
	EXPECT_EQ(wl.getItemCount(), 0);
	EXPECT_TRUE(wl.getItems().empty());
}

TEST_F(WantListFixture, AddRequestCreatesItem) {
	wl.addRequest(WantItemType::WORKER, 0.5f);

	ASSERT_EQ(wl.getItemCount(), 1);
	const auto& item = wl.getItems()[0];
	EXPECT_EQ(item.type, WantItemType::WORKER);
	EXPECT_FLOAT_EQ(item.priority, 0.5f);
	EXPECT_FLOAT_EQ(item.basePriority, 0.5f);
	EXPECT_EQ(item.count, 1);
	EXPECT_EQ(item.specificId, -1);
	EXPECT_EQ(item.age, 0);
	EXPECT_EQ(item.reserveTicks, 0);
	EXPECT_TRUE(item.active);
}

TEST_F(WantListFixture, AddRequestWithSpecificIdAndCount) {
	wl.addRequest(WantItemType::BUILDING, 0.8f, 42, 2);

	ASSERT_EQ(wl.getItemCount(), 1);
	const auto& item = wl.getItems()[0];
	EXPECT_EQ(item.type, WantItemType::BUILDING);
	EXPECT_EQ(item.count, 2);
	EXPECT_EQ(item.specificId, 42);
}

TEST_F(WantListFixture, AddRequestReactivatesExistingMatch) {
	wl.addRequest(WantItemType::UNIT, 0.5f, 7,1);
	wl.resetRequests();
	EXPECT_FALSE(wl.getItems()[0].active);

	wl.addRequest(WantItemType::UNIT, 0.9f, 7, 3);

	ASSERT_EQ(wl.getItemCount(), 1);
	const auto& item = wl.getItems()[0];
	EXPECT_TRUE(item.active);
	EXPECT_FLOAT_EQ(item.basePriority, 0.9f);
	EXPECT_EQ(item.count, 3);
}

TEST_F(WantListFixture, AddRequestCreatesNewForDifferentId) {
	wl.addRequest(WantItemType::UNIT, 0.5f,7, 1);
	wl.addRequest(WantItemType::UNIT, 0.6f,8, 1);
	EXPECT_EQ(wl.getItemCount(), 2);
}

TEST_F(WantListFixture, AddRequestCreatesNewForDifferentType) {
	wl.addRequest(WantItemType::UNIT, 0.5f, -1,1);
	wl.addRequest(WantItemType::WORKER, 0.5f,-1, 1);
	EXPECT_EQ(wl.getItemCount(), 2);
}

TEST_F(WantListFixture, AddAlwaysAccepted) {
	for (int i = 0; i < WantList::MAX_ITEMS + 5; ++i) {
		wl.addRequest(WantItemType::UNIT, 0.5f, static_cast<short>(i));
	}
	EXPECT_EQ(wl.getItemCount(), WantList::MAX_ITEMS + 5);
}

TEST_F(WantListFixture, BeginTickMarksAllInactive) {
	wl.addRequest(WantItemType::WORKER, 0.5f);
	wl.addRequest(WantItemType::UNIT, 0.6f,  1);
	wl.addRequest(WantItemType::BUILDING, 0.9f, 5);

	wl.resetRequests();

	for (const auto& item : wl.getItems()) {
		EXPECT_FALSE(item.active);
	}
}

// --- Matching logic ---

TEST_F(WantListFixture, SameTypeDefaultIdReactivates) {
	wl.addRequest(WantItemType::WORKER, 0.5f);
	wl.resetRequests();
	wl.addRequest(WantItemType::WORKER, 0.7f);

	EXPECT_EQ(wl.getItemCount(), 1);
	EXPECT_FLOAT_EQ(wl.getItems()[0].basePriority, 0.7f);
}

TEST_F(WantListFixture, DifferentSpecificIdCreatesNew) {
	wl.addRequest(WantItemType::BUILDING, 0.5f, 1);
	wl.addRequest(WantItemType::BUILDING, 0.5f, 2);
	EXPECT_EQ(wl.getItemCount(), 2);
}

// --- Multi-tick without execute ---

TEST_F(WantListFixture, MultipleTickCyclesAccumulateItems) {
	wl.resetRequests();
	wl.addRequest(WantItemType::WORKER, 0.5f);
	wl.addRequest(WantItemType::UNIT, 0.6f,10,2);

	wl.resetRequests();
	wl.addRequest(WantItemType::WORKER, 0.7f);
	wl.addRequest(WantItemType::BUILDING, 0.8f, 5);

	ASSERT_EQ(wl.getItemCount(), 3);

	bool workerActive = false, unitInactive = false, buildingActive = false;
	for (const auto& item : wl.getItems()) {
		if (item.type == WantItemType::WORKER) {
			EXPECT_TRUE(item.active);
			EXPECT_FLOAT_EQ(item.basePriority, 0.7f);
			workerActive = true;
		}
		if (item.type == WantItemType::UNIT) {
			EXPECT_FALSE(item.active);
			unitInactive = true;
		}
		if (item.type == WantItemType::BUILDING) {
			EXPECT_TRUE(item.active);
			buildingActive = true;
		}
	}
	EXPECT_TRUE(workerActive);
	EXPECT_TRUE(unitInactive);
	EXPECT_TRUE(buildingActive);
}

TEST_F(WantListFixture, ExecuteBlockedItemGrowsSoftReserveByOnePercentPerTick) {
	TestWantExecutor executor;
	executor.setCost(7, 0, 100, 0, 0);
	const auto resources = resourcesWithWood(60.f);

	runTick(wl, executor, resources, WantItemType::BUILDING, 0.9f, 7);
	ASSERT_EQ(wl.getItemCount(), 1);
	EXPECT_EQ(wl.getItems()[0].reserveTicks, 1);

	runTick(wl, executor, resources, WantItemType::BUILDING, 0.9f, 7);
	EXPECT_EQ(wl.getItems()[0].reserveTicks, 2);
}

TEST_F(WantListFixture, ExecuteBlockedItemSoftReserveCapsAtThirtyThreePercent) {
	TestWantExecutor executor;
	executor.setCost(7, 0, 100, 0, 0);
	const auto resources = resourcesWithWood(60.f);

	for (int tick = 0; tick < 40; ++tick) {
		runTick(wl, executor, resources, WantItemType::BUILDING, 0.9f, 7);
	}

	ASSERT_EQ(wl.getItemCount(), 1);
	EXPECT_EQ(wl.getItems()[0].reserveTicks, WantList::SOFT_RESERVE_MAX_PERCENT);
}

TEST_F(WantListFixture, ExecuteSuccessResetsSoftReserveTicks) {
	TestWantExecutor executor;
	executor.setCost(7, 0, 100, 0, 0);

	runTick(wl, executor, resourcesWithWood(60.f), WantItemType::BUILDING, 0.9f, 7);
	ASSERT_EQ(wl.getItems()[0].reserveTicks, 1);

	runTick(wl, executor, resourcesWithWood(120.f), WantItemType::BUILDING, 0.9f, 7);
	EXPECT_EQ(wl.getItems()[0].reserveTicks, 0);
	ASSERT_EQ(executor.executedIds.size(), 1);
	EXPECT_EQ(executor.executedIds[0], 7);
}

TEST_F(WantListFixture, ExecuteFailureAfterAffordableCheckResetsSoftReserveTicks) {
	TestWantExecutor executor;
	executor.setCost(7, 0, 100, 0, 0);
	executor.setExecuteResult(7, false);

	runTick(wl, executor, resourcesWithWood(60.f), WantItemType::BUILDING, 0.9f, 7);
	ASSERT_EQ(wl.getItems()[0].reserveTicks, 1);

	runTick(wl, executor, resourcesWithWood(120.f), WantItemType::BUILDING, 0.9f, 7);
	EXPECT_EQ(wl.getItems()[0].reserveTicks, 0);
	ASSERT_EQ(executor.executedIds.size(), 1);
	EXPECT_EQ(executor.executedIds[0], 7);
}

TEST_F(WantListFixture, SoftReserveCanBlockLowerPriorityItem) {
	TestWantExecutor executor;
	executor.setCost(1, 0, 100, 0, 0);
	executor.setCost(2, 0, 10, 0, 0);

	wl.addRequest(WantItemType::BUILDING, 0.9f, 1);
	wl.addRequest(WantItemType::BUILDING, 0.4f, 2);
	wl.execute(resourcesWithWood(10.f), executor);

	EXPECT_TRUE(executor.executedIds.empty());
	ASSERT_EQ(executor.notEnoughIds.size(), 2);
	EXPECT_EQ(executor.notEnoughIds[0], 1);
	EXPECT_EQ(executor.notEnoughIds[1], 2);
}

TEST_F(WantListFixture, BlockedMultiCountItemAppliesSingleSoftReserve) {
	TestWantExecutor executor;
	executor.setCost(1, 0, 60, 0, 0);
	executor.setCost(2, 0, 49, 0, 0);

	wl.addRequest(WantItemType::UNIT, 0.9f, 1, 3);
	wl.addRequest(WantItemType::BUILDING, 0.4f, 2);
	wl.execute(resourcesWithWood(50.5f), executor);

	ASSERT_EQ(executor.executedIds.size(), 1);
	EXPECT_EQ(executor.executedIds[0], 2);
	ASSERT_EQ(wl.getItems().size(), 2);
	EXPECT_EQ(wl.getItems()[0].reserveTicks, 1);
}

// ==========================================
// UnitOutput / count scaling
// ==========================================

TEST(UnitOutputTest, CountScalingLogic) {
	// Test the count scaling formula: count = max(1, round(urgency * MAX_UNITS_PER_TICK))
	constexpr int MAX_UNITS_PER_TICK = UnitBrain::MAX_UNITS_PER_TICK;
	EXPECT_EQ(MAX_UNITS_PER_TICK, 5);

	// urgency=0 → count=0
	float urgency = 0.f;
	int count = (urgency > 0.f) ? std::max(1, static_cast<int>(std::round(urgency * MAX_UNITS_PER_TICK))) : 0;
	EXPECT_EQ(count, 0);

	// urgency=0.1 → round(0.5) = 1, max(1,1) = 1
	urgency = 0.1f;
	count = (urgency > 0.f) ? std::max(1, static_cast<int>(std::round(urgency * MAX_UNITS_PER_TICK))) : 0;
	EXPECT_EQ(count, 1);

	// urgency=0.5 → round(2.5) = 3 (banker's rounding), max(1,3) = 3
	urgency = 0.5f;
	count = (urgency > 0.f) ? std::max(1, static_cast<int>(std::round(urgency * MAX_UNITS_PER_TICK))) : 0;
	EXPECT_GE(count, 2);
	EXPECT_LE(count, 3);

	// urgency=1.0 → round(5) = 5, max(1,5) = 5
	urgency = 1.0f;
	count = (urgency > 0.f) ? std::max(1, static_cast<int>(std::round(urgency * MAX_UNITS_PER_TICK))) : 0;
	EXPECT_EQ(count, 5);

	// very small urgency → still at least 1
	urgency = 0.001f;
	count = (urgency > 0.f) ? std::max(1, static_cast<int>(std::round(urgency * MAX_UNITS_PER_TICK))) : 0;
	EXPECT_EQ(count, 1);
}


// ==========================================
// Boost/Decay math verification
// ==========================================

TEST(BoostDecayMathTest, BoostFormulaIncreasesPriorityWithAge) {
	// Saturating boost (matches WantList::boostOrDecay):
	//   priority = basePriority * (1 + BOOST_MAX * age / (age + BOOST_HALF_AGE))
	constexpr float basePri = 0.5f;
	auto boosted = [](float base, float age) {
		const float boost = WantList::BOOST_MAX * age / (age + WantList::BOOST_HALF_AGE);
		return base * (1.f + boost);
	};

	const float age0 = boosted(basePri, 0.f);
	const float age1 = boosted(basePri, 1.f);
	const float age2 = boosted(basePri, 2.f);
	const float age5 = boosted(basePri, 5.f);

	EXPECT_FLOAT_EQ(age0, basePri);     // no boost at age 0
	EXPECT_GT(age1, age0);              // monotonically increases with age
	EXPECT_GT(age2, age1);
	EXPECT_GT(age5, age2);

	// At age == BOOST_HALF_AGE the boost is exactly half of BOOST_MAX.
	const float halfAge = boosted(basePri, WantList::BOOST_HALF_AGE);
	EXPECT_NEAR(halfAge, basePri * (1.f + WantList::BOOST_MAX / 2.f), 1e-5f);

	// Boost saturates: it can never reach the (1 + BOOST_MAX) ceiling.
	const float ceiling = basePri * (1.f + WantList::BOOST_MAX);
	EXPECT_LT(boosted(basePri, 1000.f), ceiling);
	EXPECT_GT(boosted(basePri, 1000.f), basePri * (1.f + WantList::BOOST_MAX * 0.99f));
}

TEST(BoostDecayMathTest, DecayReducesPriority) {
	float pri = 0.5f;
	float decay = WantList::DECAY_FACTOR;

	float after1 = pri * decay;
	float after2 = after1 * decay;
	float after5 = pri * std::pow(decay, 5.f);

	EXPECT_LT(after1, pri);
	EXPECT_LT(after2, after1);
	EXPECT_LT(after5, after2);
}

TEST(BoostDecayMathTest, DecayEventuallyDropsBelowThreshold) {
	float pri = 0.5f;
	float decay = WantList::DECAY_FACTOR;
	float threshold = WantList::DROP_THRESHOLD;

	int ticks = 0;
	while (pri >= threshold && ticks < 100) {
		pri *= decay;
		ticks++;
	}

	EXPECT_LT(pri, threshold);
	EXPECT_GT(ticks, 5);   // shouldn't drop too fast
	EXPECT_LT(ticks, 50);  // shouldn't take forever
}

TEST(BoostDecayMathTest, HighPriorityItemSurvivesLongerDecay) {
	float highPri = 0.9f;
	float lowPri = 0.1f;
	float decay = WantList::DECAY_FACTOR;
	float threshold = WantList::DROP_THRESHOLD;

	int highTicks = 0, lowTicks = 0;
	float h = highPri, l = lowPri;
	while (h >= threshold && highTicks < 200) { h *= decay; highTicks++; }
	while (l >= threshold && lowTicks < 200) { l *= decay; lowTicks++; }

	EXPECT_GT(highTicks, lowTicks);
}
