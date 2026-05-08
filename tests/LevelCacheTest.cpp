#include "pch.h"
#include "../game/src/env/bucket/levels/LevelCache.h"
#include "../game/src/env/bucket/levels/LevelCache.cpp"
#include "../game/src/env/GridCalculator.h"
#include "../game/include/Urho3D/Math/Vector3.h"

class LevelCacheFixture : public ::testing::Test {
public:
	static GridCalculator* gc;
	static LevelCache* cache;

	static constexpr unsigned short RESOLUTION = 16;
	static constexpr float SIZE = 32.f;
	static constexpr float MAX_DISTANCE = 10.f;

protected:
	static void SetUpTestCase() {
		gc = new GridCalculator(RESOLUTION, SIZE);
		cache = new LevelCache(MAX_DISTANCE, gc);
	}

	static void TearDownTestCase() {
		delete cache;
		delete gc;
	}
};

GridCalculator* LevelCacheFixture::gc = nullptr;
LevelCache* LevelCacheFixture::cache = nullptr;

TEST_F(LevelCacheFixture, GetResolution) {
	EXPECT_EQ(cache->getResolution(), RESOLUTION);
}

TEST_F(LevelCacheFixture, GetMaxDistance) {
	EXPECT_FLOAT_EQ(cache->getMaxDistance(), MAX_DISTANCE);
}

TEST_F(LevelCacheFixture, ZeroRadiusReturnsSingleElement) {
	const auto* result = cache->get(0.f, 0);
	ASSERT_NE(result, nullptr);
	EXPECT_EQ(result->size(), 1);
	EXPECT_EQ((*result)[0], 0);
}

TEST_F(LevelCacheFixture, ZeroRadiusFromCenter) {
	const int centerIndex = 8 * RESOLUTION + 8; // center of grid
	const auto* result = cache->get(0.f, centerIndex);
	ASSERT_NE(result, nullptr);
	EXPECT_EQ(result->size(), 1);
	EXPECT_EQ((*result)[0], 0); // offset 0 = self
}

TEST_F(LevelCacheFixture, LargerRadiusReturnsMoreElements) {
	const int centerIndex = 8 * RESOLUTION + 8;
	const auto* small = cache->get(1.f, centerIndex);
	const auto* large = cache->get(5.f, centerIndex);
	ASSERT_NE(small, nullptr);
	ASSERT_NE(large, nullptr);
	EXPECT_GE(large->size(), small->size());
}

TEST_F(LevelCacheFixture, ResultIsSorted) {
	const int centerIndex = 8 * RESOLUTION + 8;
	const auto* result = cache->get(5.f, centerIndex);
	ASSERT_NE(result, nullptr);
	EXPECT_TRUE(std::is_sorted(result->begin(), result->end()));
}

TEST_F(LevelCacheFixture, SelfIsAlwaysIncluded) {
	const int centerIndex = 8 * RESOLUTION + 8;
	const auto* result = cache->get(3.f, centerIndex);
	ASSERT_NE(result, nullptr);
	bool containsZero = std::find(result->begin(), result->end(), 0) != result->end();
	EXPECT_TRUE(containsZero);
}

TEST_F(LevelCacheFixture, CenterGetsSafePathNoClipping) {
	// Center cell is far from edges, should get full unclipped result
	const int centerIndex = 8 * RESOLUTION + 8;
	const auto* fromCenter = cache->get(3.f, centerIndex);

	// Same radius from UShortVector2 overload
	const auto cords = gc->getCords(centerIndex);
	const auto* fromCords = cache->get(3.f, cords);

	ASSERT_NE(fromCenter, nullptr);
	ASSERT_NE(fromCords, nullptr);
	EXPECT_EQ(fromCenter->size(), fromCords->size());
}

TEST_F(LevelCacheFixture, EdgeCellClipsResults) {
	// Corner cell (0,0) should have fewer neighbors than center
	const auto* fromCorner = cache->get(5.f, 0);
	const int centerIndex = 8 * RESOLUTION + 8;
	const auto* fromCenter = cache->get(5.f, centerIndex);
	ASSERT_NE(fromCorner, nullptr);
	ASSERT_NE(fromCenter, nullptr);
	EXPECT_LT(fromCorner->size(), fromCenter->size());
}

TEST_F(LevelCacheFixture, UShortVector2OverloadMatchesIntOverload) {
	const int index = 5 * RESOLUTION + 7;
	const auto cords = gc->getCords(index);

	const auto* fromInt = cache->get(4.f, index);
	const auto* fromVec = cache->get(4.f, cords);

	ASSERT_NE(fromInt, nullptr);
	ASSERT_NE(fromVec, nullptr);
	EXPECT_EQ(fromInt->size(), fromVec->size());
	for (size_t i = 0; i < fromInt->size(); ++i) {
		EXPECT_EQ((*fromInt)[i], (*fromVec)[i]);
	}
}

TEST_F(LevelCacheFixture, RadiusClampedToMax) {
	const int centerIndex = 8 * RESOLUTION + 8;
	// Radius beyond maxDistance should clamp
	const auto* atMax = cache->get(MAX_DISTANCE, centerIndex);
	const auto* beyondMax = cache->get(MAX_DISTANCE * 2.f, centerIndex);
	ASSERT_NE(atMax, nullptr);
	ASSERT_NE(beyondMax, nullptr);
	EXPECT_EQ(atMax->size(), beyondMax->size());
}

TEST_F(LevelCacheFixture, AllReturnedIndexesAreValidOffsets) {
	const int centerIndex = 8 * RESOLUTION + 8;
	const auto* result = cache->get(5.f, centerIndex);
	ASSERT_NE(result, nullptr);
	for (const auto offset : *result) {
		int absoluteIndex = centerIndex + offset;
		EXPECT_GE(absoluteIndex, 0);
		EXPECT_LT(absoluteIndex, RESOLUTION * RESOLUTION);
	}
}

TEST_F(LevelCacheFixture, SymmetricResults) {
	// For a center cell, positive and negative offsets should be symmetric
	const int centerIndex = 8 * RESOLUTION + 8;
	const auto* result = cache->get(3.f, centerIndex);
	ASSERT_NE(result, nullptr);
	for (const auto offset : *result) {
		if (offset == 0) continue;
		bool hasNegative = std::find(result->begin(), result->end(), -offset) != result->end();
		EXPECT_TRUE(hasNegative) << "Missing symmetric offset for " << offset;
	}
}

// --- Larger resolution fixture to stress type boundaries ---

class LevelCacheLargeFixture : public ::testing::Test {
public:
	static GridCalculator* gc;
	static LevelCache* cache;

	static constexpr unsigned short RESOLUTION = 512;
	static constexpr float SIZE = 1024.f;
	static constexpr float MAX_DISTANCE = 60.f;

protected:
	static void SetUpTestCase() {
		gc = new GridCalculator(RESOLUTION, SIZE);
		cache = new LevelCache(MAX_DISTANCE, gc);
	}

	static void TearDownTestCase() {
		delete cache;
		delete gc;
	}
};

GridCalculator* LevelCacheLargeFixture::gc = nullptr;
LevelCache* LevelCacheLargeFixture::cache = nullptr;

// maxShift is unsigned short — verify it doesn't wrap negative
TEST_F(LevelCacheLargeFixture, MaxShiftNeverWrapsNegative) {
	// Query from cell (0,0) with large radius — maxShift used in bounds check
	// If maxShift were signed short and overflowed, the >= 0 check would wrongly pass
	const auto* result = cache->get(MAX_DISTANCE, 0);
	ASSERT_NE(result, nullptr);
	// All absolute indices must be valid
	for (const auto offset : *result) {
		int absIdx = 0 + offset;
		EXPECT_GE(absIdx, 0) << "Negative absolute index from corner: offset=" << offset;
		EXPECT_LT(absIdx, RESOLUTION * RESOLUTION);
	}
}

// Offsets fit in short — with resolution=256, max offset magnitude is ~256*34=8704
TEST_F(LevelCacheLargeFixture, OffsetsWithinShortRange) {
	const int centerIndex = 128 * RESOLUTION + 128;
	const auto* result = cache->get(MAX_DISTANCE, centerIndex);
	ASSERT_NE(result, nullptr);
	for (const auto offset : *result) {
		EXPECT_GE(offset, static_cast<short>(-32768));
		EXPECT_LE(offset, static_cast<short>(32767));
		// Verify offset doesn't silently wrap — absolute index must be near center
		int absIdx = centerIndex + offset;
		EXPECT_TRUE(gc->isValidIndex(absIdx))
			<< "Invalid absolute index: center=" << centerIndex << " offset=" << offset;
	}
}

// UShortVector2 coords near unsigned short boundary (255, 255)
TEST_F(LevelCacheLargeFixture, TopRightCornerUShortBoundary) {
	// Cell (511, 511) — max values for unsigned short coords at resolution=512
	Urho3D::UShortVector2 corner(RESOLUTION - 1, RESOLUTION - 1);
	const auto* result = cache->get(5.f, corner);
	ASSERT_NE(result, nullptr);
	EXPECT_GT(result->size(), 0u);

	int cornerIndex = (RESOLUTION - 1) * RESOLUTION + (RESOLUTION - 1);
	for (const auto offset : *result) {
		int absIdx = cornerIndex + offset;
		EXPECT_TRUE(gc->isValidIndex(absIdx))
			<< "Out of bounds from (255,255): offset=" << offset << " absIdx=" << absIdx;
	}
}

// UShortVector2 (0, 0) — tests subtraction from unsigned short doesn't underflow in bounds check
TEST_F(LevelCacheLargeFixture, OriginCornerUShortSubtraction) {
	Urho3D::UShortVector2 origin(0, 0);
	const auto* result = cache->get(MAX_DISTANCE, origin);
	ASSERT_NE(result, nullptr);
	EXPECT_GT(result->size(), 0u);

	for (const auto offset : *result) {
		int absIdx = 0 + offset;
		EXPECT_GE(absIdx, 0) << "Negative index from origin: offset=" << offset;
		EXPECT_TRUE(gc->isValidIndex(absIdx));
	}
}

// Test edge row — (0, 128): x=0 means subtraction of maxShift from 0
TEST_F(LevelCacheLargeFixture, EdgeRowXZero) {
	Urho3D::UShortVector2 edge(0, 128);
	const auto* resultEdge = cache->get(5.f, edge);
	Urho3D::UShortVector2 center(128, 128);
	const auto* resultCenter = cache->get(5.f, center);
	ASSERT_NE(resultEdge, nullptr);
	ASSERT_NE(resultCenter, nullptr);
	// Edge should have fewer results due to clipping
	EXPECT_LT(resultEdge->size(), resultCenter->size());
}

// Test edge column — (128, 0): y=0 means subtraction of maxShift from 0
TEST_F(LevelCacheLargeFixture, EdgeColumnYZero) {
	Urho3D::UShortVector2 edge(128, 0);
	const auto* result = cache->get(5.f, edge);
	ASSERT_NE(result, nullptr);
	EXPECT_GT(result->size(), 0u);

	int edgeIndex = 128 * RESOLUTION + 0;
	for (const auto offset : *result) {
		int absIdx = edgeIndex + offset;
		EXPECT_TRUE(gc->isValidIndex(absIdx))
			<< "Invalid from edge column: offset=" << offset;
	}
}

// ShortVector2 shifts: verify center+shift doesn't produce invalid coords
// This tests the static_cast<short> in the isValidIndex call
TEST_F(LevelCacheLargeFixture, ShiftAdditionDoesNotOverflow) {
	// Cell (1, 1) — close to edge, large radius forces many shifts to be clipped
	Urho3D::UShortVector2 nearEdge(1, 1);
	const auto* result = cache->get(MAX_DISTANCE, nearEdge);
	ASSERT_NE(result, nullptr);

	int nearEdgeIndex = 1 * RESOLUTION + 1;
	for (const auto offset : *result) {
		int absIdx = nearEdgeIndex + offset;
		EXPECT_GE(absIdx, 0);
		EXPECT_LT(absIdx, RESOLUTION * RESOLUTION);
	}
}

// Cell at (RESOLUTION-1, 1) — tests positive x shift overflow
TEST_F(LevelCacheLargeFixture, FarEdgeXMaxShiftOverflow) {
	Urho3D::UShortVector2 farEdge(RESOLUTION - 1, 128);
	const auto* result = cache->get(MAX_DISTANCE, farEdge);
	ASSERT_NE(result, nullptr);

	int idx = (RESOLUTION - 1) * RESOLUTION + 128;
	for (const auto offset : *result) {
		int absIdx = idx + offset;
		EXPECT_TRUE(gc->isValidIndex(absIdx))
			<< "Invalid from far x edge: offset=" << offset;
	}
}

// Verify all four corners produce valid results with max radius
TEST_F(LevelCacheLargeFixture, AllCornersMaxRadius) {
	const unsigned short lastIdx = RESOLUTION - 1;
	Urho3D::UShortVector2 corners[] = {
		{0, 0},
		{0, lastIdx},
		{lastIdx, 0},
		{lastIdx, lastIdx}
	};

	for (const auto& corner : corners) {
		const auto* result = cache->get(MAX_DISTANCE, corner);
		ASSERT_NE(result, nullptr);
		EXPECT_GT(result->size(), 0u);

		int cornerIndex = corner.x_ * RESOLUTION + corner.y_;
		for (const auto offset : *result) {
			int absIdx = cornerIndex + offset;
			EXPECT_TRUE(gc->isValidIndex(absIdx))
				<< "Corner (" << corner.x_ << "," << corner.y_ << ") offset=" << offset;
		}
	}
}

// Consistency: calling get() twice with same params returns same pointer (cached)
TEST_F(LevelCacheLargeFixture, ConsistentCachingCenter) {
	const int centerIndex = 128 * RESOLUTION + 128;
	const auto* first = cache->get(5.f, centerIndex);
	const auto* second = cache->get(5.f, centerIndex);
	// Center cell uses fast path (returns levels[index].indexes directly)
	EXPECT_EQ(first, second);
}

// Monotonic growth across multiple radii
TEST_F(LevelCacheLargeFixture, MonotonicGrowthAcrossRadii) {
	const int centerIndex = 128 * RESOLUTION + 128;
	size_t prevSize = 0;
	for (float r = 0.f; r <= MAX_DISTANCE; r += 1.f) {
		const auto* result = cache->get(r, centerIndex);
		ASSERT_NE(result, nullptr);
		EXPECT_GE(result->size(), prevSize)
			<< "Size decreased at radius=" << r;
		prevSize = result->size();
	}
}
