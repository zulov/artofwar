#include "pch.h"

#include <algorithm>
#include <limits>

#include "env/GridCalculator.h"
#include "env/GridCalculatorProvider.h"
#include "env/influence/map/InfluenceMap.h"
#include "env/influence/map/InfluenceTemplateProvider.h"

#include "../game/src/env/influence/map/InfluenceTemplateProvider.cpp"
#include "../game/src/env/influence/map/InfluenceMap.cpp"



// Testable subclass exposing internals for direct manipulation
class TestableInfluenceMap : public InfluenceMap {
public:
	TestableInfluenceMap(GridCalculator* calculator)
		: InfluenceMap(calculator, 0.f) {
	}

	~TestableInfluenceMap() override = default;

	void setValues(const std::vector<float>& vals) {
		assert(vals.size() == arraySize);
		std::copy(vals.begin(), vals.end(), kernelValues);
		valuesCalculateNeeded = false;
	}

	void setMinMax(float minVal, float maxVal) {
		min = minVal;
		max = maxVal;
		minMaxInited = true;
	}

	unsigned int getArraySize() const { return arraySize; }
};

class CumulateErrorsFixture : public ::testing::Test {
protected:
	// 4x4 grid, size=8 -> fieldSize=2
	static constexpr unsigned short RES = 4;
	static constexpr float SIZE = 8.f;
	static constexpr int ARRAY_SIZE = RES * RES;

	TestableInfluenceMap* map;
	float intersection[ARRAY_SIZE];

	void SetUp() override {
		map = new TestableInfluenceMap(GridCalculatorProvider::get(RES, SIZE));
	}

	void TearDown() override {
		delete map;
	}

	void resetIntersection() {
		std::fill_n(intersection, ARRAY_SIZE, 0.f);
	}

	// Helper: set uniform values and min/max
	void setupMap(const std::vector<float>& vals) {
		map->setValues(vals);
		float mn = *std::min_element(vals.begin(), vals.end());
		float mx = *std::max_element(vals.begin(), vals.end());
		map->setMinMax(mn, mx);
	}
};

TEST(InfluenceTemplateProviderTest, ReusesTemplateForMatchingParameters) {
	const auto* first = InfluenceTemplateProvider::get(0.5f, 4);
	const auto* second = InfluenceTemplateProvider::get(0.5f, 4);

	EXPECT_EQ(first, second);
}

TEST(InfluenceTemplateProviderTest, SeparatesTemplatesWithDifferentParameters) {
	const auto* originalTemplate = InfluenceTemplateProvider::get(0.25f, 2);
	const auto originalCenterValue = originalTemplate[12];
	const auto* coefficientTemplate = InfluenceTemplateProvider::get(1.f, 4);
	const auto* levelTemplate = InfluenceTemplateProvider::get(0.5f, 1);

	EXPECT_EQ(originalTemplate, InfluenceTemplateProvider::get(0.25f, 2));
	EXPECT_FLOAT_EQ(originalTemplate[12], originalCenterValue);
	EXPECT_NE(coefficientTemplate, levelTemplate);
}

TEST(InfluenceTemplateProviderTest, GeneratesExpectedKernelWeights) {
	const auto* values = InfluenceTemplateProvider::get(0.5f, 1);

	EXPECT_FLOAT_EQ(values[0], 0.5f);
	EXPECT_FLOAT_EQ(values[1], 2.f / 3.f);
	EXPECT_FLOAT_EQ(values[4], 1.f);
	EXPECT_FLOAT_EQ(values[8], 0.5f);
}

// --- Positive percent: seek high values ---

TEST_F(CumulateErrorsFixture, PositivePercentSeeksHighValues) {
	// Values: cell 0=0.0, cell 1=0.5, cell 2=1.0, rest=0.0
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.f;
	vals[1] = 0.5f;
	vals[2] = 1.0f;
	setupMap(vals);
	resetIntersection();

	map->cumulateErrors(0.8f, intersection);

	// percent=0.8, min=0, max=1, invDiff=1
	// val = (max - cellVal) * invDiff * percent = (1 - cellVal) * 0.8
	// cell 0: val = 0.8,  err = 0.64
	// cell 1: val = 0.4,  err = 0.16
	// cell 2: val = 0.0,  err = 0.00
	EXPECT_NEAR(intersection[0], 0.64f, 1e-5f);
	EXPECT_NEAR(intersection[1], 0.16f, 1e-5f);
	EXPECT_NEAR(intersection[2], 0.00f, 1e-5f);

	// Highest value cell has lowest error -> seeks high values
	EXPECT_LT(intersection[2], intersection[1]);
	EXPECT_LT(intersection[1], intersection[0]);
}

// --- Negative percent: seek low values ---

TEST_F(CumulateErrorsFixture, NegativePercentSeeksLowValues) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.f;
	vals[1] = 0.5f;
	vals[2] = 1.0f;
	setupMap(vals);
	resetIntersection();

	map->cumulateErrors(-0.8f, intersection);

	// percent=-0.8, min=0, max=1
	// val = (cellVal - min) * invDiff * percent = cellVal * (-0.8)
	// err = val^2 = (cellVal * 0.8)^2
	// cell 0: err = 0.00
	// cell 1: err = 0.16
	// cell 2: err = 0.64
	EXPECT_NEAR(intersection[0], 0.00f, 1e-5f);
	EXPECT_NEAR(intersection[1], 0.16f, 1e-5f);
	EXPECT_NEAR(intersection[2], 0.64f, 1e-5f);

	// Lowest value cell has lowest error -> seeks low values
	EXPECT_LT(intersection[0], intersection[1]);
	EXPECT_LT(intersection[1], intersection[2]);
}

// --- Zero percent: ignore map ---

TEST_F(CumulateErrorsFixture, ZeroPercentIgnoresMap) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.f;
	vals[1] = 0.5f;
	vals[2] = 1.0f;
	setupMap(vals);
	resetIntersection();

	map->cumulateErrors(0.0f, intersection);

	// All errors should be zero regardless of cell values
	for (int i = 0; i < ARRAY_SIZE; ++i) {
		EXPECT_FLOAT_EQ(intersection[i], 0.f) << "cell " << i;
	}
}

// --- Weight magnitude controls influence strength ---

TEST_F(CumulateErrorsFixture, LargerWeightProducesLargerErrors) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.f; // worst cell for seek-high
	vals[1] = 1.f; // best cell -> makes the map non-uniform (max>min) so errors are computed
	setupMap(vals);

	resetIntersection();
	map->cumulateErrors(0.3f, intersection);
	float errSmallWeight = intersection[0];

	resetIntersection();
	map->cumulateErrors(0.9f, intersection);
	float errLargeWeight = intersection[0];

	EXPECT_GT(errLargeWeight, errSmallWeight);
}

TEST_F(CumulateErrorsFixture, LargerNegativeWeightProducesLargerErrors) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 1.0f; // worst cell for seek-low
	setupMap(vals);

	resetIntersection();
	map->cumulateErrors(-0.3f, intersection);
	float errSmallWeight = intersection[0];

	resetIntersection();
	map->cumulateErrors(-0.9f, intersection);
	float errLargeWeight = intersection[0];

	EXPECT_GT(errLargeWeight, errSmallWeight);
}

// --- Cumulation: errors from multiple maps accumulate ---

TEST_F(CumulateErrorsFixture, ErrorsAccumulateAcrossMultipleCalls) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.5f;
	setupMap(vals);
	resetIntersection();

	map->cumulateErrors(0.8f, intersection);
	float afterFirst = intersection[0];

	map->cumulateErrors(0.8f, intersection);
	float afterSecond = intersection[0];

	EXPECT_NEAR(afterSecond, afterFirst * 2.f, 1e-5f);
}

// --- Unseen cells (huge sentinel) stay effectively excluded by magnitude ---
// NOTE: the explicit `if (*intersection < maxVal)` skip was removed; every cell
// is now processed. A pre-seeded huge sentinel still dominates because the added
// squared error is negligible vs FLT_MAX, so such cells remain ranked last.

TEST_F(CumulateErrorsFixture, UnseenSentinelStaysLargest) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 1.0f;
	setupMap(vals);
	resetIntersection();

	// Mark cell 0 as unseen with the sentinel used by VisibilityManager.
	constexpr auto maxVal = std::numeric_limits<float>::max() - 1.f;
	intersection[0] = maxVal + 1.f;

	map->cumulateErrors(0.8f, intersection);

	// Cell 0 remains huge -> still effectively excluded from selection. Because
	// maxVal is so close to FLT_MAX that (maxVal + 1.f) == FLT_MAX and adding a
	// small squared error saturates, the value stays at the maximum rather than
	// growing. Assert it stays maximal (>=) instead of strictly greater.
	EXPECT_GE(intersection[0], maxVal);
}

// --- Uniform map returns false ---

TEST_F(CumulateErrorsFixture, UniformMapReturnsFalse) {
	std::vector<float> vals(ARRAY_SIZE, 0.5f);
	setupMap(vals);
	resetIntersection();

	bool result = map->cumulateErrors(0.8f, intersection);
	EXPECT_FALSE(result);
}

// --- Non-uniform map returns true ---

TEST_F(CumulateErrorsFixture, NonUniformMapReturnsTrue) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 1.0f;
	setupMap(vals);
	resetIntersection();

	bool result = map->cumulateErrors(0.8f, intersection);
	EXPECT_TRUE(result);
}

// --- Combined scenario: two maps, seek high buildings + seek low gathering ---

TEST_F(CumulateErrorsFixture, CombinedSeekHighAndSeekLow) {
	// Simulate building placement: seek high buildings, avoid gathering
	// Cell 0: high buildings (0.9), no gathering (0.0) -> best candidate
	// Cell 1: high buildings (0.9), high gathering (0.8) -> worse due to gathering
	// Cell 2: low buildings (0.1), no gathering (0.0) -> bad due to no buildings
	// Cell 3: low buildings (0.1), high gathering (0.9) -> worst

	// First map: "buildings" - seek high
	std::vector<float> bldVals(ARRAY_SIZE, 0.f);
	bldVals[0] = 0.9f;
	bldVals[1] = 0.9f;
	bldVals[2] = 0.1f;
	bldVals[3] = 0.1f;
	setupMap(bldVals);
	resetIntersection();
	map->cumulateErrors(0.9f, intersection); // seek high buildings

	// Second map: "gathering" - seek low (avoid)
	std::vector<float> gatherVals(ARRAY_SIZE, 0.f);
	gatherVals[0] = 0.0f;
	gatherVals[1] = 0.8f;
	gatherVals[2] = 0.0f;
	gatherVals[3] = 0.9f;
	setupMap(gatherVals);
	map->cumulateErrors(-0.6f, intersection); // avoid gathering

	// Cell 0 should win (high buildings, no gathering)
	EXPECT_LT(intersection[0], intersection[1]);
	EXPECT_LT(intersection[0], intersection[2]);
	EXPECT_LT(intersection[0], intersection[3]);

	// Cell 3 should be worst (low buildings, high gathering)
	EXPECT_GT(intersection[3], intersection[0]);
	EXPECT_GT(intersection[3], intersection[1]);
	EXPECT_GT(intersection[3], intersection[2]);
}

// --- Regression: min != 0 must produce correct errors (guards branch/flip bugs) ---
// These cases are critical: when min == 0 several incorrect formulations still
// coincidentally yield the right numbers. A non-zero min disambiguates them.
// Reference (original intended formula), invDiff = 1 / (max - min):
//   seek-high (percent >= 0): err = ((max - cellVal) * invDiff * percent)^2
//   seek-low  (percent <  0): err = ((cellVal - min) * invDiff * percent)^2

TEST_F(CumulateErrorsFixture, NonZeroMinSeekHighExactErrors) {
	// min=10, max=20 -> invDiff = 0.1
	std::vector<float> vals(ARRAY_SIZE, 10.f); // baseline = min
	vals[0] = 10.f;
	vals[1] = 12.5f;
	vals[2] = 15.f;
	vals[3] = 17.5f;
	vals[4] = 20.f; // = max
	setupMap(vals);
	resetIntersection();

	map->cumulateErrors(0.8f, intersection);

	// err = ((max - cellVal) * 0.1 * 0.8)^2
	EXPECT_NEAR(intersection[0], 0.6400f, 1e-5f); // (10*0.08)^2
	EXPECT_NEAR(intersection[1], 0.3600f, 1e-5f); // (7.5*0.08)^2
	EXPECT_NEAR(intersection[2], 0.1600f, 1e-5f); // (5*0.08)^2
	EXPECT_NEAR(intersection[3], 0.0400f, 1e-5f); // (2.5*0.08)^2
	EXPECT_NEAR(intersection[4], 0.0000f, 1e-5f); // (0)^2

	// Ranking: highest cell -> lowest error (seek high)
	EXPECT_LT(intersection[4], intersection[3]);
	EXPECT_LT(intersection[3], intersection[2]);
	EXPECT_LT(intersection[2], intersection[1]);
	EXPECT_LT(intersection[1], intersection[0]);
}

TEST_F(CumulateErrorsFixture, NonZeroMinSeekLowExactErrors) {
	// min=10, max=20 -> invDiff = 0.1
	std::vector<float> vals(ARRAY_SIZE, 10.f);
	vals[0] = 10.f; // = min
	vals[1] = 12.5f;
	vals[2] = 15.f;
	vals[3] = 17.5f;
	vals[4] = 20.f; // = max
	setupMap(vals);
	resetIntersection();

	map->cumulateErrors(-0.8f, intersection);

	// err = ((cellVal - min) * 0.1 * 0.8)^2  (sign of percent cancels under square)
	EXPECT_NEAR(intersection[0], 0.0000f, 1e-5f); // (0)^2
	EXPECT_NEAR(intersection[1], 0.0400f, 1e-5f); // (2.5*0.08)^2
	EXPECT_NEAR(intersection[2], 0.1600f, 1e-5f); // (5*0.08)^2
	EXPECT_NEAR(intersection[3], 0.3600f, 1e-5f); // (7.5*0.08)^2
	EXPECT_NEAR(intersection[4], 0.6400f, 1e-5f); // (10*0.08)^2

	// Ranking: lowest cell -> lowest error (seek low)
	EXPECT_LT(intersection[0], intersection[1]);
	EXPECT_LT(intersection[1], intersection[2]);
	EXPECT_LT(intersection[2], intersection[3]);
	EXPECT_LT(intersection[3], intersection[4]);
}

TEST_F(CumulateErrorsFixture, NegativeMinRangeSeekLowExactErrors) {
	// min=-4, max=4 -> invDiff = 0.125, stresses negative min in both branches
	std::vector<float> vals(ARRAY_SIZE, -4.f);
	vals[0] = -4.f; // = min
	vals[1] = 0.f;
	vals[2] = 2.f;
	vals[3] = 4.f; // = max
	setupMap(vals);
	resetIntersection();

	map->cumulateErrors(-0.5f, intersection);

	// err = ((cellVal - min) * 0.125 * 0.5)^2
	EXPECT_NEAR(intersection[0], 0.00000000f, 1e-6f); // (0)^2
	EXPECT_NEAR(intersection[1], 0.06250000f, 1e-6f); // (4*0.0625)^2
	EXPECT_NEAR(intersection[2], 0.14062500f, 1e-6f); // (6*0.0625)^2
	EXPECT_NEAR(intersection[3], 0.25000000f, 1e-6f); // (8*0.0625)^2

	EXPECT_LT(intersection[0], intersection[1]);
	EXPECT_LT(intersection[1], intersection[2]);
	EXPECT_LT(intersection[2], intersection[3]);
}

// --- Symmetry: positive and negative with same magnitude produce mirrored rankings ---

TEST_F(CumulateErrorsFixture, PositiveAndNegativeAreSymmetric) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.0f;
	vals[1] = 0.5f;
	vals[2] = 1.0f;
	setupMap(vals);

	resetIntersection();
	map->cumulateErrors(0.8f, intersection);
	float posErr0 = intersection[0]; // high error (cell is low, wants high)
	float posErr2 = intersection[2]; // low error (cell is high, wants high)

	resetIntersection();
	map->cumulateErrors(-0.8f, intersection);
	float negErr0 = intersection[0]; // low error (cell is low, wants low)
	float negErr2 = intersection[2]; // high error (cell is high, wants low)

	// Errors should be mirrored
	EXPECT_NEAR(posErr0, negErr2, 1e-5f);
	EXPECT_NEAR(posErr2, negErr0, 1e-5f);
}

TEST(InfluenceMapRegression, GetKernelMaxIdxsHandlesSmallMap) {
	TestableInfluenceMap map(GridCalculatorProvider::get(4, 8.f));
	map.update(0, 1.f);

	const auto indexes = map.getKernelMaxIdxs();
	ASSERT_EQ(indexes.size(), 10u);
	EXPECT_EQ(indexes[0], 0u);
}

TEST(InfluenceMapRegression, ImmediateModeUpdatesRawAndResetClearsMap) {
	InfluenceMap map(GridCalculatorProvider::get(4, 8.f), 0.f);
	map.update(0, 3.f);

	EXPECT_FLOAT_EQ(map.getRaw(0), 3.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 3.f);

	map.reset();

	EXPECT_FLOAT_EQ(map.getRaw(0), 0.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 0.f);
}

TEST(InfluenceMapRegression, ResetDecaysRawAndRebuildsKernelCache) {
	InfluenceMap map(GridCalculatorProvider::get(4, 8.f), 0.f, true);
	map.update(0, 2.f);

	EXPECT_FLOAT_EQ(map.getRaw(0), 0.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 0.f);
	map.reset();

	EXPECT_FLOAT_EQ(map.getRaw(0), 2.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 2.f);
	map.reset();

	EXPECT_FLOAT_EQ(map.getRaw(0), 1.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 1.f);
}

TEST(InfluenceMapRegression, ResetToZeroDropsValuesBelowThreshold) {
	InfluenceMap map(GridCalculatorProvider::get(4, 8.f), 0.f, true);
	map.update(0, 0.00005f);

	EXPECT_FLOAT_EQ(map.getKernel(0), 0.f);
	map.reset();

	EXPECT_FLOAT_EQ(map.getKernel(0), 0.00005f);
	map.resetToZero();

	EXPECT_FLOAT_EQ(map.getRaw(0), 0.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 0.f);
}

TEST(InfluenceMapRegression, HistoryResetDecaysRawAndInvalidatesKernel) {
	InfluenceMap map(GridCalculatorProvider::get(4, 8.f), 0.f, true);
	map.update(0, 4.f);

	EXPECT_FLOAT_EQ(map.getRaw(0), 0.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 0.f);

	map.reset();

	EXPECT_FLOAT_EQ(map.getRaw(0), 4.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 4.f);

	map.reset();

	EXPECT_FLOAT_EQ(map.getRaw(0), 2.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 2.f);
}

TEST(InfluenceMapRegression, BufferedHistoryResetMovesPendingIntoRawAndRebuildsKernel) {
	InfluenceMap map(GridCalculatorProvider::get(4, 8.f), 0.f, true);
	map.update(0, 4.f);

	EXPECT_FLOAT_EQ(map.getRaw(0), 0.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 0.f);

	map.reset();

	EXPECT_FLOAT_EQ(map.getRaw(0), 4.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 4.f);

	map.reset();

	EXPECT_FLOAT_EQ(map.getRaw(0), 2.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 2.f);
}

TEST(InfluenceMapRegression, BufferedHistoryResetAddsPendingOnTopOfDecayedRaw) {
	InfluenceMap map(GridCalculatorProvider::get(4, 8.f), 0.f, true);
	map.update(0, 4.f);
	map.reset();
	map.update(0, 6.f);

	map.reset();

	EXPECT_FLOAT_EQ(map.getRaw(0), 8.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 8.f);
}

TEST(InfluenceMapRegression, BufferedHistoryResetToZeroKeepsPendingAndDropsSmallRawValues) {
	InfluenceMap map(GridCalculatorProvider::get(4, 8.f), 0.f, true);
	map.update(0, 0.00005f);
	map.reset();
	map.update(0, 5.f);

	map.resetToZero();

	EXPECT_FLOAT_EQ(map.getRaw(0), 0.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 0.f);

	map.reset();

	EXPECT_FLOAT_EQ(map.getRaw(0), 5.f);
	EXPECT_FLOAT_EQ(map.getKernel(0), 5.f);
}

TEST(InfluenceMapRegression, GetCenterUsesRawTerminalLayer) {
	TestableInfluenceMap map(GridCalculatorProvider::get(4, 8.f));
	map.update(4, 1.f);
	map.update(5, 2.f);

	const auto center = map.getCenter();
	ASSERT_TRUE(center.has_value());

	GridCalculator calculator(4, 8.f);
	EXPECT_EQ(*center, calculator.getCenter(5));
}

TEST(InfluenceMapRegression, GetCenterRebuildsQuadAfterReset) {
	TestableInfluenceMap map(GridCalculatorProvider::get(4, 8.f));
	GridCalculator calculator(4, 8.f);

	map.update(0, 1.f);
	ASSERT_EQ(map.getCenter(), calculator.getCenter(0));

	map.reset();
	map.update(15, 1.f);
	EXPECT_EQ(map.getCenter(), calculator.getCenter(15));
}
