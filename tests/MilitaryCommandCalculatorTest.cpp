#include "pch.h"

#include "player/ai/MilitaryCommandCalculator.h"
#include "utils/OtherUtils.h"

namespace {
	MilitaryCenterSnapshot makeCenter(const Urho3D::Vector2& pos, float confidence = 1.f) {
		MilitaryCenterSnapshot s;
		s.available = true;
		s.position = pos;
		s.confidence = confidence;
		return s;
	}
}

TEST(MilitaryCommandCalculatorTest, ChoosesEnemyArmyForExamplePressures) {
	MilitaryCommandCalculator calc(100.f);
	MilitaryOutput output;
	output.centerPairPressure.fill(0.f);
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_BUILDING, MilitaryCenterIdx::ENEMY_ARMY)] = 0.7f;
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::ENEMY_ECON)] = 0.1f;
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::ENEMY_ARMY)] = 0.05f;

	std::array<MilitaryCenterSnapshot, MILITARY_CENTER_COUNT> centers{};
	centers[castC(MilitaryCenterIdx::OUR_ARMY)] = makeCenter({0.f, 0.f});
	centers[castC(MilitaryCenterIdx::OUR_BUILDING)] = makeCenter({0.f, 0.f});
	centers[castC(MilitaryCenterIdx::ENEMY_ARMY)] = makeCenter({10.f, 0.f});
	centers[castC(MilitaryCenterIdx::ENEMY_ECON)] = makeCenter({30.f, 0.f});

	auto result = calc.calculate({0.f, 0.f}, centers, output);
	EXPECT_EQ(result.best.center, MilitaryCenterIdx::ENEMY_ARMY);
	EXPECT_GT(result.best.score, 0.f);
}

TEST(MilitaryCommandCalculatorTest, IgnoresUnavailableCenters) {
	MilitaryCommandCalculator calc(100.f);
	MilitaryOutput output;
	output.centerPairPressure.fill(0.f);
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::ENEMY_ARMY)] = 1.f;

	std::array<MilitaryCenterSnapshot, MILITARY_CENTER_COUNT> centers{};
	centers[castC(MilitaryCenterIdx::OUR_ARMY)] = makeCenter({0.f, 0.f});
	centers[castC(MilitaryCenterIdx::ENEMY_ARMY)] = {};

	auto result = calc.calculate({0.f, 0.f}, centers, output);
	EXPECT_EQ(result.best.center, MilitaryCenterIdx::OUR_ARMY);
	EXPECT_FLOAT_EQ(result.best.score, 0.f);
}

TEST(MilitaryCommandCalculatorTest, ConfidenceScalesInfluence) {
	MilitaryCommandCalculator calc(100.f);
	MilitaryOutput output;
	output.centerPairPressure.fill(0.f);
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::ENEMY_ECON)] = 1.f;
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_BUILDING, MilitaryCenterIdx::ENEMY_BUILDING)] = 0.2f;

	std::array<MilitaryCenterSnapshot, MILITARY_CENTER_COUNT> centers{};
	centers[castC(MilitaryCenterIdx::OUR_ARMY)] = makeCenter({0.f, 0.f}, 1.f);
	centers[castC(MilitaryCenterIdx::OUR_BUILDING)] = makeCenter({0.f, 0.f}, 0.1f);
	centers[castC(MilitaryCenterIdx::ENEMY_ECON)] = makeCenter({10.f, 0.f});
	centers[castC(MilitaryCenterIdx::ENEMY_BUILDING)] = makeCenter({20.f, 0.f});

	auto result = calc.calculate({0.f, 0.f}, centers, output);
	EXPECT_EQ(result.best.center, MilitaryCenterIdx::ENEMY_ECON);
	EXPECT_GT(result.scores[castC(MilitaryCenterIdx::ENEMY_ECON)], result.scores[castC(MilitaryCenterIdx::ENEMY_BUILDING)]);
}

TEST(MilitaryCommandCalculatorTest, DistanceRadiusCreatesZeroClosenessOutsideRange) {
	MilitaryCommandCalculator calc(10.f);
	MilitaryOutput output;
	output.centerPairPressure.fill(0.f);
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::ENEMY_ARMY)] = 1.f;

	std::array<MilitaryCenterSnapshot, MILITARY_CENTER_COUNT> centers{};
	centers[castC(MilitaryCenterIdx::OUR_ARMY)] = makeCenter({0.f, 0.f});
	centers[castC(MilitaryCenterIdx::ENEMY_ARMY)] = makeCenter({100.f, 0.f});

	auto result = calc.calculate({0.f, 0.f}, centers, output);
	EXPECT_FLOAT_EQ(result.scores[castC(MilitaryCenterIdx::ENEMY_ARMY)], 0.f);
}

TEST(MilitaryCommandCalculatorTest, ReturnsFirstAvailableWhenAllScoresZero) {
	MilitaryCommandCalculator calc(10.f);
	MilitaryOutput output;
	output.centerPairPressure.fill(0.f);

	std::array<MilitaryCenterSnapshot, MILITARY_CENTER_COUNT> centers{};
	centers[castC(MilitaryCenterIdx::ENEMY_ARMY)] = makeCenter({100.f, 0.f});

	auto result = calc.calculate({0.f, 0.f}, centers, output);
	EXPECT_EQ(result.best.center, MilitaryCenterIdx::ENEMY_ARMY);
	EXPECT_FLOAT_EQ(result.best.score, 0.f);
}
