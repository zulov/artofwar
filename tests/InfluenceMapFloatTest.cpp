#include "pch.h"

#include <algorithm>
#include <limits>

#include "env/GridCalculator.h"
#include "env/influence/map/InfluenceMapFloat.h"

// Testable subclass exposing internals for direct manipulation
class TestableInfluenceMapFloat : public InfluenceMapFloat {
public:
	TestableInfluenceMapFloat(unsigned short resolution, float size, float coef, char level)
		: InfluenceMapFloat(resolution, size, coef, level, 0.f, createTemplateV(coef, level)) {
	}

	~TestableInfluenceMapFloat() override = default;

	void setValues(const std::vector<float>& vals) {
		assert(vals.size() == arraySize);
		std::copy(vals.begin(), vals.end(), values);
		valuesCalculateNeeded = false;
	}

	void setMinMax(float minVal, float maxVal) {
		min = minVal;
		max = maxVal;
		minMaxInited = true;
	}

	unsigned int getArraySize() const { return arraySize; }
};

class CumulateErrosFixture : public ::testing::Test {
protected:
	// 4x4 grid, size=8 -> fieldSize=2
	static constexpr unsigned short RES = 4;
	static constexpr float SIZE = 8.f;
	static constexpr int ARRAY_SIZE = RES * RES;

	TestableInfluenceMapFloat* map;
	float intersection[ARRAY_SIZE];

	void SetUp() override {
		map = new TestableInfluenceMapFloat(RES, SIZE, 1.f, 1);
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

// --- Positive percent: seek high values ---

TEST_F(CumulateErrosFixture, PositivePercentSeeksHighValues) {
	// Values: cell 0=0.0, cell 1=0.5, cell 2=1.0, rest=0.0
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.f;
	vals[1] = 0.5f;
	vals[2] = 1.0f;
	setupMap(vals);
	resetIntersection();

	map->cumulateErros(0.8f, intersection);

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

TEST_F(CumulateErrosFixture, NegativePercentSeeksLowValues) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.f;
	vals[1] = 0.5f;
	vals[2] = 1.0f;
	setupMap(vals);
	resetIntersection();

	map->cumulateErros(-0.8f, intersection);

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

TEST_F(CumulateErrosFixture, ZeroPercentIgnoresMap) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.f;
	vals[1] = 0.5f;
	vals[2] = 1.0f;
	setupMap(vals);
	resetIntersection();

	map->cumulateErros(0.0f, intersection);

	// All errors should be zero regardless of cell values
	for (int i = 0; i < ARRAY_SIZE; ++i) {
		EXPECT_FLOAT_EQ(intersection[i], 0.f) << "cell " << i;
	}
}

// --- Weight magnitude controls influence strength ---

TEST_F(CumulateErrosFixture, LargerWeightProducesLargerErrors) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.f; // worst cell for seek-high
	setupMap(vals);

	resetIntersection();
	map->cumulateErros(0.3f, intersection);
	float errSmallWeight = intersection[0];

	resetIntersection();
	map->cumulateErros(0.9f, intersection);
	float errLargeWeight = intersection[0];

	EXPECT_GT(errLargeWeight, errSmallWeight);
}

TEST_F(CumulateErrosFixture, LargerNegativeWeightProducesLargerErrors) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 1.0f; // worst cell for seek-low
	setupMap(vals);

	resetIntersection();
	map->cumulateErros(-0.3f, intersection);
	float errSmallWeight = intersection[0];

	resetIntersection();
	map->cumulateErros(-0.9f, intersection);
	float errLargeWeight = intersection[0];

	EXPECT_GT(errLargeWeight, errSmallWeight);
}

// --- Cumulation: errors from multiple maps accumulate ---

TEST_F(CumulateErrosFixture, ErrorsAccumulateAcrossMultipleCalls) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.5f;
	setupMap(vals);
	resetIntersection();

	map->cumulateErros(0.8f, intersection);
	float afterFirst = intersection[0];

	map->cumulateErros(0.8f, intersection);
	float afterSecond = intersection[0];

	EXPECT_NEAR(afterSecond, afterFirst * 2.f, 1e-5f);
}

// --- Skips cells marked as unseen (MAX value) ---

TEST_F(CumulateErrosFixture, SkipsUnseenCells) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 1.0f;
	setupMap(vals);
	resetIntersection();

	// Mark cell 0 as unseen
	constexpr auto maxVal = std::numeric_limits<float>::max() - 1.f;
	intersection[0] = maxVal + 1.f;

	map->cumulateErros(0.8f, intersection);

	// Cell 0 should not have been modified (still above maxVal)
	EXPECT_GT(intersection[0], maxVal);
}

// --- Uniform map returns false ---

TEST_F(CumulateErrosFixture, UniformMapReturnsFalse) {
	std::vector<float> vals(ARRAY_SIZE, 0.5f);
	setupMap(vals);
	resetIntersection();

	bool result = map->cumulateErros(0.8f, intersection);
	EXPECT_FALSE(result);
}

// --- Non-uniform map returns true ---

TEST_F(CumulateErrosFixture, NonUniformMapReturnsTrue) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 1.0f;
	setupMap(vals);
	resetIntersection();

	bool result = map->cumulateErros(0.8f, intersection);
	EXPECT_TRUE(result);
}

// --- Combined scenario: two maps, seek high buildings + seek low gathering ---

TEST_F(CumulateErrosFixture, CombinedSeekHighAndSeekLow) {
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
	map->cumulateErros(0.9f, intersection); // seek high buildings

	// Second map: "gathering" - seek low (avoid)
	std::vector<float> gatherVals(ARRAY_SIZE, 0.f);
	gatherVals[0] = 0.0f;
	gatherVals[1] = 0.8f;
	gatherVals[2] = 0.0f;
	gatherVals[3] = 0.9f;
	setupMap(gatherVals);
	map->cumulateErros(-0.6f, intersection); // avoid gathering

	// Cell 0 should win (high buildings, no gathering)
	EXPECT_LT(intersection[0], intersection[1]);
	EXPECT_LT(intersection[0], intersection[2]);
	EXPECT_LT(intersection[0], intersection[3]);

	// Cell 3 should be worst (low buildings, high gathering)
	EXPECT_GT(intersection[3], intersection[0]);
	EXPECT_GT(intersection[3], intersection[1]);
	EXPECT_GT(intersection[3], intersection[2]);
}

// --- Symmetry: positive and negative with same magnitude produce mirrored rankings ---

TEST_F(CumulateErrosFixture, PositiveAndNegativeAreSymmetric) {
	std::vector<float> vals(ARRAY_SIZE, 0.f);
	vals[0] = 0.0f;
	vals[1] = 0.5f;
	vals[2] = 1.0f;
	setupMap(vals);

	resetIntersection();
	map->cumulateErros(0.8f, intersection);
	float posErr0 = intersection[0]; // high error (cell is low, wants high)
	float posErr2 = intersection[2]; // low error (cell is high, wants high)

	resetIntersection();
	map->cumulateErros(-0.8f, intersection);
	float negErr0 = intersection[0]; // low error (cell is low, wants low)
	float negErr2 = intersection[2]; // high error (cell is high, wants low)

	// Errors should be mirrored
	EXPECT_NEAR(posErr0, negErr2, 1e-5f);
	EXPECT_NEAR(posErr2, negErr0, 1e-5f);
}
