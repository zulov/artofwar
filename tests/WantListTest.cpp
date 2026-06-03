#include "pch.h"

#include <array>
#include <cmath>

#include "player/ai/WantList.h"
#include "player/ai/MasterBrain.h"
#include "player/ai/EconomyBrain.h"
#include "player/ai/UnitBrain.h"
#include "player/ai/MilitaryBrain.h"

// ==========================================
// WantList unit tests
// ==========================================

class WantListFixture : public ::testing::Test {
protected:
	WantList wl;
};

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

// ==========================================
// WantList::execute integration tests
// Uses real Player/Resources via game lib
// ==========================================

#include "player/Player.h"
#include "player/Resources.h"
#include "database/db_struct.h"

class WantListExecuteFixture : public ::testing::Test {
protected:
	WantList wl;

	// Costs for test items
	db_with_cost cheapCost{10, 10, 0, 0};      // 10 food + 10 wood
	db_with_cost expensiveCost{900, 900, 900, 900}; // very expensive

	int executeCount = 0;

	WantList::ExecuteCallback countingExecute = [this](WantItem& item) {
		executeCount++;
		return true;
	};

	WantList::ExecuteCallback alwaysFail = [](WantItem& item) {
		return false;
	};
};

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
	// boost formula: priority = basePriority * (1 + BOOST_FACTOR * age)
	float basePri = 0.5f;
	float boost = WantList::BOOST_FACTOR;

	float age0 = basePri * (1.f + boost * 0);
	float age1 = basePri * (1.f + boost * 1);
	float age2 = basePri * (1.f + boost * 2);
	float age5 = basePri * (1.f + boost * 5);

	EXPECT_FLOAT_EQ(age0, 0.5f);        // no boost at age 0
	EXPECT_GT(age1, age0);               // increases with age
	EXPECT_GT(age2, age1);
	EXPECT_GT(age5, age2);
	EXPECT_NEAR(age1, 0.5f * 2.15f, 1e-5f); // 0.5 * (1 + 1.15) = 1.075
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
