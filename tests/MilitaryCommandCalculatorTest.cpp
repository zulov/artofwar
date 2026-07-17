#include "pch.h"

#include "player/ai/MilitaryCommandCalculator.h"
#include "utils/OtherUtils.h"

namespace {
	std::optional<Urho3D::Vector2> makeCenter(const Urho3D::Vector2& pos) {
		return pos;
	}
}

TEST(MilitaryCommandCalculatorTest, MilitaryCenterPairOrderMatchesPressureEncoding) {
	EXPECT_EQ(militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::OUR_ECON), 0u);
	EXPECT_EQ(militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::BATTLE), 5u);
	EXPECT_EQ(militaryCenterPairIndex(MilitaryCenterIdx::OUR_ECON, MilitaryCenterIdx::BATTLE), 10u);
	EXPECT_EQ(militaryCenterPairIndex(MilitaryCenterIdx::ENEMY_BUILDING, MilitaryCenterIdx::BATTLE), 20u);

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

	std::array<std::optional<Urho3D::Vector2>, MILITARY_CENTER_COUNT> centers{};
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

	std::array<std::optional<Urho3D::Vector2>, MILITARY_CENTER_COUNT> centers{};
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

	std::array<std::optional<Urho3D::Vector2>, MILITARY_CENTER_COUNT> centers{};
	centers[castC(MilitaryCenterIdx::OUR_ARMY)] = makeCenter({0.f, 0.f});
	centers[castC(MilitaryCenterIdx::OUR_BUILDING)] = makeCenter({0.f, 0.f});
	centers[castC(MilitaryCenterIdx::ENEMY_ECON)] = makeCenter({10.f, 0.f});
	centers[castC(MilitaryCenterIdx::ENEMY_BUILDING)] = makeCenter({20.f, 0.f});

	auto result = calc.calculate({0.f, 0.f}, centers, output);
	EXPECT_EQ(result.best.center, MilitaryCenterIdx::ENEMY_ECON);
	EXPECT_GT(result.scores[castC(MilitaryCenterIdx::ENEMY_ECON)], result.scores[castC(MilitaryCenterIdx::ENEMY_BUILDING)]);
}

TEST(MilitaryCommandCalculatorTest, PressureIsWeightedBySourceClosenessNotTargetDistance) {
	// The calculator weights each pressure pair by how close the *source* center is
	// to the unit, not by the target's distance. OUR_ARMY sits on the unit (distance 0),
	// so its closeness is 1 and the pressure toward the far-away ENEMY_ARMY is preserved
	// at full strength regardless of how distant the enemy is.
	MilitaryCommandCalculator calc(10.f);
	MilitaryOutput output;
	output.centerPairPressure.fill(0.f);
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_ARMY, MilitaryCenterIdx::ENEMY_ARMY)] = 1.f;

	std::array<std::optional<Urho3D::Vector2>, MILITARY_CENTER_COUNT> centers{};
	centers[castC(MilitaryCenterIdx::OUR_ARMY)] = makeCenter({0.f, 0.f});
	centers[castC(MilitaryCenterIdx::ENEMY_ARMY)] = makeCenter({100.f, 0.f});

	auto result = calc.calculate({0.f, 0.f}, centers, output);
	EXPECT_FLOAT_EQ(result.scores[castC(MilitaryCenterIdx::ENEMY_ARMY)],
	                1.f / static_cast<float>(MILITARY_CENTER_COUNT - 1));
	EXPECT_EQ(result.best.center, MilitaryCenterIdx::ENEMY_ARMY);
}

TEST(MilitaryCommandCalculatorTest, ReturnsFirstAvailableWhenAllScoresZero) {
	MilitaryCommandCalculator calc(10.f);
	MilitaryOutput output;
	output.centerPairPressure.fill(0.f);

	std::array<std::optional<Urho3D::Vector2>, MILITARY_CENTER_COUNT> centers{};
	centers[castC(MilitaryCenterIdx::ENEMY_ARMY)] = makeCenter({100.f, 0.f});

	auto result = calc.calculate({0.f, 0.f}, centers, output);
	EXPECT_EQ(result.best.center, MilitaryCenterIdx::ENEMY_ARMY);
	EXPECT_FLOAT_EQ(result.best.score, 0.f);
}

TEST(MilitaryCommandCalculatorTest, SourceOutsideRadiusContributesZeroPressure) {
	// When the pressure source center is beyond the radius, its closeness clamps to 0,
	// so it contributes nothing to any target score even if the pressure value is high.
	MilitaryCommandCalculator calc(10.f);
	MilitaryOutput output;
	output.centerPairPressure.fill(0.f);
	output.centerPairPressure[militaryCenterPairIndex(MilitaryCenterIdx::OUR_ECON, MilitaryCenterIdx::ENEMY_ARMY)] = 1.f;

	std::array<std::optional<Urho3D::Vector2>, MILITARY_CENTER_COUNT> centers{};
	// Source (OUR_ECON) is 100 units from the unit at origin -> outside radius 10.
	centers[castC(MilitaryCenterIdx::OUR_ECON)] = makeCenter({100.f, 0.f});
	centers[castC(MilitaryCenterIdx::ENEMY_ARMY)] = makeCenter({5.f, 0.f});

	auto result = calc.calculate({0.f, 0.f}, centers, output);
	EXPECT_FLOAT_EQ(result.scores[castC(MilitaryCenterIdx::ENEMY_ARMY)], 0.f);
}

TEST(MilitaryCommandCalculatorTest, MaximumScoreIsNormalizedToOne) {
	MilitaryCommandCalculator calc(100.f);
	MilitaryOutput output;
	output.centerPairPressure.fill(1.f);

	std::array<std::optional<Urho3D::Vector2>, MILITARY_CENTER_COUNT> centers{};
	for (size_t i = 0; i < MILITARY_CENTER_COUNT; ++i) {
		centers[i] = makeCenter({0.f, 0.f});
	}

	auto result = calc.calculate({0.f, 0.f}, centers, output);
	EXPECT_FLOAT_EQ(result.scores[castC(MilitaryCenterIdx::BATTLE)],
	                MAX_MILITARY_UNIT_PRESSURE);
}
