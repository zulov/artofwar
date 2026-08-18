#include "pch.h"

#include "Game.h"
#include "player/ai/AiHistory.cpp"

class AiHistoryFixture : public ::testing::Test {
protected:
	AiHistory history;

	void SetUp() override {
		Game::init();
	}

	void TearDown() override {
		Game::dispose();
	}

	void advanceTicks(unsigned int count) {
		for (unsigned int i = 0; i < count; ++i) {
			Game::getFrameInfo()->countFrame();
		}
	}

	unsigned int currentTick() {
		return Game::getFrameInfo()->getTotalTicks();
	}
};

TEST_F(AiHistoryFixture, RecencyScoreZeroWhenEmpty) {
	advanceTicks(10);
	EXPECT_FLOAT_EQ(history.recencyScore({AiActionType::CREATE_WORKER}), 0.f);
}

TEST_F(AiHistoryFixture, RecencyScoreOneForJustCreated) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
	// age = 0, score = 1.0 - 0/1200 = 1.0
	EXPECT_FLOAT_EQ(history.recencyScore({AiActionType::CREATE_WORKER}), 1.f);
}

TEST_F(AiHistoryFixture, RecencyScoreDecaysWithAge) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
	EXPECT_FLOAT_EQ(history.recencyScore({AiActionType::CREATE_WORKER}), 1.f);
	advanceTicks(50);
	// age = 50, lookback = 1200, score = 1.0 - 50/1200.
	EXPECT_NEAR(history.recencyScore({AiActionType::CREATE_WORKER}), 1.f - 50.f / 1200.f, 1e-5f);
}

TEST_F(AiHistoryFixture, RecencyScoreIgnoresOldEntries) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
	advanceTicks(1201);
	// age = 1201 > lookback = 1200, entry is outside window
	EXPECT_FLOAT_EQ(history.recencyScore({AiActionType::CREATE_WORKER}), 0.f);
}

TEST_F(AiHistoryFixture, RecencyScoreIgnoresOtherTypes) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_UNIT, AiActionResult::SUCCESS);
	EXPECT_FLOAT_EQ(history.recencyScore({AiActionType::CREATE_WORKER}), 0.f);
}

TEST_F(AiHistoryFixture, RecencyScoreAccumulatesMultipleSuccesses) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS); // tick 1
	advanceTicks(10);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS); // tick 11
	advanceTicks(10);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS); // tick 21

	// now = 21, lookback = 1200
	// entry at tick 1:  1.0 - 20/1200
	// entry at tick 11: 1.0 - 10/1200
	// entry at tick 21: 1.0 - 0/1200 = 1.0
	// total = 2.975
	EXPECT_NEAR(history.recencyScore({AiActionType::CREATE_WORKER}), 2.975f, 1e-5f);
}

TEST_F(AiHistoryFixture, RetainsEntriesAcrossLegacyBufferCapacity) {
	constexpr int EVENTS = 129;
	advanceTicks(1);
	for (int i = 0; i < EVENTS; ++i) {
		history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
		if (i + 1 < EVENTS) { advanceTicks(1); }
	}

	const float expected = EVENTS - (128.f * 129.f) / (2.f * 1200.f);
	EXPECT_NEAR(history.recencyScore({AiActionType::CREATE_WORKER}), expected, 1e-5f);
}

TEST_F(AiHistoryFixture, RecencyScoreOrderOverload) {
	advanceTicks(1);
	history.addOrder(AiOrderType::COLLECT_RESOURCE_0, AiOrderResult::SUCCESS);
	EXPECT_FLOAT_EQ(history.recencyScore({AiOrderType::COLLECT_RESOURCE_0}), 1.f);
	EXPECT_FLOAT_EQ(history.recencyScore({AiOrderType::ATTACK_ECON}), 0.f);
}

TEST_F(AiHistoryFixture, CompositeScoresSumTypesFromTheSameHistoryWindow) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
	history.addAction(AiActionType::CREATE_UNIT, AiActionResult::NO_ENOUGH_RESOURCES);
	history.addOrder(AiOrderType::ATTACK_ECON, AiOrderResult::SUCCESS);
	history.addOrder(AiOrderType::ATTACK_BUILDING, AiOrderResult::NO_CENTER_POSITION);

	EXPECT_FLOAT_EQ(history.recencyScore({AiActionType::CREATE_WORKER, AiActionType::CREATE_UNIT}), 1.f);
	EXPECT_FLOAT_EQ(history.failureScore({AiActionType::CREATE_WORKER, AiActionType::CREATE_UNIT}), 1.f);
	EXPECT_FLOAT_EQ(history.recencyScore({AiOrderType::ATTACK_ECON, AiOrderType::ATTACK_BUILDING}), 1.f);
	EXPECT_FLOAT_EQ(history.failureScore({AiOrderType::ATTACK_ECON, AiOrderType::ATTACK_BUILDING}), 1.f);
}

TEST_F(AiHistoryFixture, ScoresReflectHistoryChanges) {
	advanceTicks(1);
	history.addOrder(AiOrderType::ATTACK_ECON, AiOrderResult::SUCCESS);
	EXPECT_FLOAT_EQ(history.recencyScore({AiOrderType::ATTACK_ECON}), 1.f);

	history.addOrder(AiOrderType::ATTACK_ECON, AiOrderResult::SUCCESS);
	EXPECT_FLOAT_EQ(history.recencyScore({AiOrderType::ATTACK_ECON}), 2.f);
}

TEST_F(AiHistoryFixture, CircularBufferOverwritesOldEntries) {
	advanceTicks(1);
	// Fill buffer beyond MAX_ENTRIES
	for (int i = 0; i < AiHistory::MAX_ENTRIES + 5; ++i) {
		history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
	}
	// All entries have score 1, so the score also verifies the circular-buffer cap.
	EXPECT_FLOAT_EQ(history.recencyScore({AiActionType::CREATE_WORKER}), AiHistory::MAX_ENTRIES);
}
