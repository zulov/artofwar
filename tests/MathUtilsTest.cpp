#include "pch.h"

#include "math/MathUtils.h"

class MathUtilsFixture : public ::testing::Test {};

// --- sqDistAs2D(float, float) ---

TEST_F(MathUtilsFixture, SqDistAs2DZero) {
	EXPECT_FLOAT_EQ(sqDistAs2D(0.f, 0.f), 0.f);
}

TEST_F(MathUtilsFixture, SqDistAs2DBasic) {
	EXPECT_FLOAT_EQ(sqDistAs2D(3.f, 4.f), 25.f);
}

TEST_F(MathUtilsFixture, SqDistAs2DNegative) {
	EXPECT_FLOAT_EQ(sqDistAs2D(-3.f, -4.f), 25.f);
}

// --- fixValue ---

TEST_F(MathUtilsFixture, FixValueInRange) {
	EXPECT_FLOAT_EQ(fixValue(5.f, 10.f), 5.f);
}

TEST_F(MathUtilsFixture, FixValueAboveMax) {
	EXPECT_FLOAT_EQ(fixValue(15.f, 10.f), 10.f);
}

TEST_F(MathUtilsFixture, FixValueBelowZero) {
	EXPECT_FLOAT_EQ(fixValue(-5.f, 10.f), 0.f);
}

TEST_F(MathUtilsFixture, FixValueAtBoundaries) {
	EXPECT_FLOAT_EQ(fixValue(0.f, 10.f), 0.f);
	EXPECT_FLOAT_EQ(fixValue(10.f, 10.f), 10.f);
}

// --- getCordsInHigher ---

TEST_F(MathUtilsFixture, GetCordsInHigherIndex0Res2) {
	auto result = getCordsInHigher(2, 0);
	EXPECT_EQ(result[0], 0);
	EXPECT_EQ(result[1], 1);
	EXPECT_EQ(result[2], 4);
	EXPECT_EQ(result[3], 5);
}

TEST_F(MathUtilsFixture, GetCordsInHigherIndex1Res2) {
	auto result = getCordsInHigher(2, 1);
	EXPECT_EQ(result[0], 2);
	EXPECT_EQ(result[1], 3);
	EXPECT_EQ(result[2], 6);
	EXPECT_EQ(result[3], 7);
}

TEST_F(MathUtilsFixture, GetCordsInHigherIndex2Res2) {
	// index=2, res=2: div=1, mod=0 -> value=2*(0+1*4)=8
	auto result = getCordsInHigher(2, 2);
	EXPECT_EQ(result[0], 8);
	EXPECT_EQ(result[1], 9);
	EXPECT_EQ(result[2], 12);
	EXPECT_EQ(result[3], 13);
}

TEST_F(MathUtilsFixture, GetCordsInHigherIndex3Res2) {
	// index=3, res=2: div=1, mod=1 -> value=2*(1+1*4)=10
	auto result = getCordsInHigher(2, 3);
	EXPECT_EQ(result[0], 10);
	EXPECT_EQ(result[1], 11);
	EXPECT_EQ(result[2], 14);
	EXPECT_EQ(result[3], 15);
}

// --- getCordsInLower ---

TEST_F(MathUtilsFixture, GetCordsInLowerBasic) {
	// 4x4 grid child (0,0) -> parent 2x2 index 0
	EXPECT_EQ(getCordsInLower(2, 4, 0), 0);
}

TEST_F(MathUtilsFixture, GetCordsInLowerChild1) {
	// child index 1: x=0, z=1 -> x/2=0, z/2=0 -> parent 0
	EXPECT_EQ(getCordsInLower(2, 4, 1), 0);
}

TEST_F(MathUtilsFixture, GetCordsInLowerChild5) {
	// child index 5: x=5/4=1, z=5%4=1 -> x/2=0, z/2=0 -> parent 0*2+0=0
	EXPECT_EQ(getCordsInLower(2, 4, 5), 0);
}

TEST_F(MathUtilsFixture, GetCordsInLowerChild15) {
	// child index 15: x=15/4=3, z=15%4=3 -> x/2=1, z/2=1 -> parent 1*2+1=3
	EXPECT_EQ(getCordsInLower(2, 4, 15), 3);
}

// --- sqRootSumError ---

TEST_F(MathUtilsFixture, SqRootSumErrorIdentical) {
	std::valarray<float> v1 = {1.f, 2.f, 3.f};
	std::valarray<float> v2 = {1.f, 2.f, 3.f};
	EXPECT_FLOAT_EQ(sqRootSumError(v1, v2), 0.f);
}

TEST_F(MathUtilsFixture, SqRootSumErrorBasic) {
	std::valarray<float> v1 = {1.f, 2.f, 3.f};
	std::valarray<float> v2 = {2.f, 3.f, 4.f};
	// diffs: 1,1,1 -> squared: 1,1,1 -> sum: 3
	EXPECT_FLOAT_EQ(sqRootSumError(v1, v2), 3.f);
}

TEST_F(MathUtilsFixture, SqRootSumErrorLargerDiffs) {
	std::valarray<float> v1 = {0.f, 0.f};
	std::valarray<float> v2 = {3.f, 4.f};
	// diffs: 3,4 -> squared: 9,16 -> sum: 25
	EXPECT_FLOAT_EQ(sqRootSumError(v1, v2), 25.f);
}

// --- calculateSize ---

TEST_F(MathUtilsFixture, CalculateSizeOdd) {
	auto result = calculateSize(3, 5);
	EXPECT_EQ(result.x_, 4);
	EXPECT_EQ(result.y_, 7);
}

TEST_F(MathUtilsFixture, CalculateSizeEven) {
	auto result = calculateSize(4, 5);
	// first = -((4-1)/2) = -1, second = 4 + (-1) = 3
	EXPECT_EQ(result.x_, 4);
	EXPECT_EQ(result.y_, 8);
}

TEST_F(MathUtilsFixture, CalculateSizeOne) {
	auto result = calculateSize(1, 0);
	EXPECT_EQ(result.x_, 0);
	EXPECT_EQ(result.y_, 1);
}

// --- sqDist with Urho3D types ---

TEST_F(MathUtilsFixture, SqDistVector2) {
	Urho3D::Vector2 a(0.f, 0.f);
	Urho3D::Vector2 b(3.f, 4.f);
	EXPECT_FLOAT_EQ(sqDist(a, b), 25.f);
}

TEST_F(MathUtilsFixture, SqDistVector3) {
	Urho3D::Vector3 a(0.f, 0.f, 0.f);
	Urho3D::Vector3 b(1.f, 2.f, 2.f);
	EXPECT_FLOAT_EQ(sqDist(a, b), 9.f);
}

TEST_F(MathUtilsFixture, SqDistVector3As2D) {
	Urho3D::Vector3 a(0.f, 100.f, 0.f);
	Urho3D::Vector3 b(3.f, 200.f, 4.f);
	// sqDistAs2D ignores y: (3-0)^2 + (4-0)^2 = 25
	EXPECT_FLOAT_EQ(sqDistAs2D(a, b), 25.f);
}

// --- Edge cases ---

TEST_F(MathUtilsFixture, SqDistAs2DLargeValues) {
	EXPECT_FLOAT_EQ(sqDistAs2D(1000.f, 1000.f), 2000000.f);
}

TEST_F(MathUtilsFixture, SqDistAs2DMixed) {
	EXPECT_FLOAT_EQ(sqDistAs2D(-3.f, 4.f), 25.f);
}

TEST_F(MathUtilsFixture, FixValueExactlyZero) {
	EXPECT_FLOAT_EQ(fixValue(0.f, 0.f), 0.f);
}

TEST_F(MathUtilsFixture, FixValueNegativeMax) {
	// value=-1 < 0 -> returns 0
	EXPECT_FLOAT_EQ(fixValue(-1.f, -5.f), 0.f);
}

TEST_F(MathUtilsFixture, SqRootSumErrorSingleElement) {
	std::valarray<float> v1 = {5.f};
	std::valarray<float> v2 = {3.f};
	EXPECT_FLOAT_EQ(sqRootSumError(v1, v2), 4.f);
}

TEST_F(MathUtilsFixture, CalculateSizeLarge) {
	auto result = calculateSize(10, 50);
	// first = -((10-1)/2) = -4, second = 10 + (-4) = 6
	EXPECT_EQ(result.x_, 46);
	EXPECT_EQ(result.y_, 56);
}

TEST_F(MathUtilsFixture, CalculateSizeZeroCentral) {
	auto result = calculateSize(3, 0);
	EXPECT_EQ(result.x_, -1);
	EXPECT_EQ(result.y_, 2);
}

TEST_F(MathUtilsFixture, GetCordsInHigherRes4) {
	// index=0, res=4: mod=0, div=0, resX2=8, value=0
	auto result = getCordsInHigher(4, 0);
	EXPECT_EQ(result[0], 0);
	EXPECT_EQ(result[1], 1);
	EXPECT_EQ(result[2], 8);
	EXPECT_EQ(result[3], 9);
}

TEST_F(MathUtilsFixture, GetCordsInLowerLargerGrid) {
	// 8x8 child -> 4x4 parent
	// child index 0 -> parent 0
	EXPECT_EQ(getCordsInLower(4, 8, 0), 0);
	// child index 7: x=7/8=0, z=7%8=7 -> x/2=0, z/2=3 -> parent 0*4+3=3
	EXPECT_EQ(getCordsInLower(4, 8, 7), 3);
	// child index 63: x=63/8=7, z=63%8=7 -> x/2=3, z/2=3 -> parent 3*4+3=15
	EXPECT_EQ(getCordsInLower(4, 8, 63), 15);
}

TEST_F(MathUtilsFixture, SqDistVector2SamePoint) {
	Urho3D::Vector2 a(5.f, 5.f);
	EXPECT_FLOAT_EQ(sqDist(a, a), 0.f);
}

TEST_F(MathUtilsFixture, SqDistVector3SamePoint) {
	Urho3D::Vector3 a(1.f, 2.f, 3.f);
	EXPECT_FLOAT_EQ(sqDist(a, a), 0.f);
}

TEST_F(MathUtilsFixture, SqDistVector2ToVector3) {
	Urho3D::Vector2 a(1.f, 2.f);
	Urho3D::Vector3 b(4.f, 99.f, 6.f); // uses x and z from b, y from a
	// sqDistAs2D(1-4, 2-6) = 9+16 = 25
	EXPECT_FLOAT_EQ(sqDist(a, b), 25.f);
}

TEST_F(MathUtilsFixture, SqDistVector3ToVector2) {
	Urho3D::Vector3 a(4.f, 99.f, 6.f);
	Urho3D::Vector2 b(1.f, 2.f);
	// sqDistAs2D(4-1, 6-2) = 9+16 = 25
	EXPECT_FLOAT_EQ(sqDist(a, b), 25.f);
}

TEST_F(MathUtilsFixture, SqDistVector3Ptr) {
	Urho3D::Vector3 a(0.f, 0.f, 0.f);
	Urho3D::Vector3 b(1.f, 1.f, 1.f);
	EXPECT_FLOAT_EQ(sqDist(&a, &b), 3.f);
}
