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
	EXPECT_FLOAT_EQ(history.recencyScore(AiActionType::CREATE_WORKER, 100), 0.f);
}

TEST_F(AiHistoryFixture, RecencyScoreOneForJustCreated) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
	// age = 0, score = 1.0 - 0/100 = 1.0
	EXPECT_FLOAT_EQ(history.recencyScore(AiActionType::CREATE_WORKER, 100), 1.f);
}

TEST_F(AiHistoryFixture, RecencyScoreDecaysWithAge) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
	advanceTicks(50);
	// age = 50, lookback = 100, score = 1.0 - 50/100 = 0.5
	EXPECT_FLOAT_EQ(history.recencyScore(AiActionType::CREATE_WORKER, 100), 0.5f);
}

TEST_F(AiHistoryFixture, RecencyScoreIgnoresOldEntries) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
	advanceTicks(101);
	// age = 101 > lookback = 100, entry is outside window
	EXPECT_FLOAT_EQ(history.recencyScore(AiActionType::CREATE_WORKER, 100), 0.f);
}

TEST_F(AiHistoryFixture, RecencyScoreIgnoresOtherTypes) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_UNIT, AiActionResult::SUCCESS);
	EXPECT_FLOAT_EQ(history.recencyScore(AiActionType::CREATE_WORKER, 100), 0.f);
}

TEST_F(AiHistoryFixture, RecencyScoreAccumulatesMultipleSuccesses) {
	advanceTicks(1);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS); // tick 1
	advanceTicks(10);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS); // tick 11
	advanceTicks(10);
	history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS); // tick 21

	// now = 21, lookback = 100
	// entry at tick 1:  1.0 - 20/100 = 0.8
	// entry at tick 11: 1.0 - 10/100 = 0.9
	// entry at tick 21: 1.0 - 0/100  = 1.0
	// total = 2.7
	EXPECT_NEAR(history.recencyScore(AiActionType::CREATE_WORKER, 100), 2.7f, 1e-5f);
}

TEST_F(AiHistoryFixture, RecencyScoreOrderOverload) {
	advanceTicks(1);
	history.addOrder(AiOrderType::COLLECT_RESOURCE_0, AiOrderResult::SUCCESS);
	EXPECT_FLOAT_EQ(history.recencyScore(AiOrderType::COLLECT_RESOURCE_0, 100), 1.f);
	EXPECT_FLOAT_EQ(history.recencyScore(AiOrderType::ATTACK_ECON, 100), 0.f);
}

TEST_F(AiHistoryFixture, CircularBufferOverwritesOldEntries) {
	advanceTicks(1);
	// Fill buffer beyond MAX_ENTRIES
	for (int i = 0; i < AiHistory::MAX_ENTRIES + 5; ++i) {
		history.addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
		advanceTicks(1);
	}
	// Should still work, count capped at MAX_ENTRIES
	EXPECT_EQ(history.getActionCount(), AiHistory::MAX_ENTRIES);
	float score = history.recencyScore(AiActionType::CREATE_WORKER, 1000);
	EXPECT_GT(score, 0.f);
}
