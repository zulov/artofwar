#include "pch.h"

#include "player/ai/MilitaryCommandCalculator.h"
#include "utils/OtherUtils.h"

namespace {
	MilitaryCenterSnapshot makeCenter(const Urho3D::Vector2& pos) {
		MilitaryCenterSnapshot s;
		s.available = true;
		s.position = pos;
		return s;
	}
}

TEST(MilitaryCommandCalculatorTest, MilitaryCenterPairOrderMatchesPressureEncoding) {
	EXPECT_EQ(militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::OUR_ECON), 0u);
	EXPECT_EQ(militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::LAST_BATTLE), 5u);
	EXPECT_EQ(militaryCenterPairIndex(MilitaryCenterIdx::OUR_ECON, MilitaryCenterIdx::LAST_BATTLE), 10u);
	EXPECT_EQ(militaryCenterPairIndex(MilitaryCenterIdx::ENEMY_BUILDING, MilitaryCenterIdx::LAST_BATTLE), 20u);

	MilitaryOutput output;
	output.centerPairPressure.fill(0.f);
	const auto idx = militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::ENEMY_ECON);
	output.centerPairPressure[idx] = 0.5f;
	EXPECT_FLOAT_EQ(output.pressure(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::ENEMY_ECON), 0.5f);
	EXPECT_FLOAT_EQ(output.pressure(MilitaryCenterIdx::ENEMY_ECON, MilitaryCenterIdx::OUR_ARMY), 0.f);
	output.centerPairPressure[idx] = -0.5f;
	EXPECT_FLOAT_EQ(output.pressure(MilitaryCenterIdx::ENEMY_ECON, MilitaryCenterIdx::OUR_ARMY), 0.5f);
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

TEST(MilitaryCommandCalculatorTest, DistanceScalesInfluence) {
	MilitaryCommandCalculator calc(100.f);
	MilitaryOutput output;
	output.centerPairPressure.fill(0.f);
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::ENEMY_ECON)] = 1.f;
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_BUILDING, MilitaryCenterIdx::ENEMY_BUILDING)] = 0.2f;

	std::array<MilitaryCenterSnapshot, MILITARY_CENTER_COUNT> centers{};
	centers[castC(MilitaryCenterIdx::OUR_ARMY)] = makeCenter({0.f, 0.f});
	centers[castC(MilitaryCenterIdx::OUR_BUILDING)] = makeCenter({0.f, 0.f});
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
