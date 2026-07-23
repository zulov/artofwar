#include "pch.h"

#include <array>
#include <limits>
#include <span>

#include "math/VectorUtils.h"

class VectorUtilsFixture : public ::testing::Test {};

TEST_F(VectorUtilsFixture, SortAndRemoveDuplicatesOrdersAndDeduplicatesValues) {
	std::vector<int> values = { 4, 2, 1, 4, 3, 2, 1 };

	sortAndRemoveDuplicates(values);

	EXPECT_EQ(values, std::vector<int>({ 1, 2, 3, 4 }));
}

TEST_F(VectorUtilsFixture, SortAndRemoveDuplicatesHandlesEmptyVector) {
	std::vector<int> values;

	sortAndRemoveDuplicates(values);

	EXPECT_TRUE(values.empty());
}

TEST_F(VectorUtilsFixture, SortIndexesOrdersValuesAscending) {
	std::vector<int> values = { 40, 10, 30, 20 };
	auto indexes = sort_indexes(std::span<int>(values), static_cast<int>(values.size()));

	EXPECT_EQ(indexes, std::vector<unsigned int>({ 1, 3, 2, 0 }));
}

TEST_F(VectorUtilsFixture, SortIndexesCanReturnOnlySmallestSubset) {
	std::vector<int> values = { 4, 1, 3, 2 };
	auto indexes = sort_indexes(std::span<int>(values), 2);

	EXPECT_EQ(indexes, std::vector<unsigned int>({ 1, 3 }));
}

TEST_F(VectorUtilsFixture, SortIndexesDescOrdersValuesDescending) {
	std::vector<int> values = { 4, 1, 3, 2 };
	auto indexes = sort_indexes_desc(std::span<int>(values), static_cast<int>(values.size()));

	EXPECT_EQ(indexes, std::vector<unsigned int>({ 0, 2, 3, 1 }));
}

TEST_F(VectorUtilsFixture, IntersectionReturnsValuesCommonToAllVectors) {
	std::vector<unsigned char> first = { 1, 2, 3, 5 };
	std::vector<unsigned char> second = { 2, 3, 4, 5 };
	std::vector<unsigned char> third = { 0, 2, 5 };
	std::vector<std::vector<unsigned char>*> ids = { &first, &second, &third };

	EXPECT_EQ(intersection(ids), std::vector<unsigned char>({ 2, 5 }));
}

TEST_F(VectorUtilsFixture, IntersectionReturnsEmptyForEmptyInput) {
	std::vector<std::vector<unsigned char>*> ids;

	EXPECT_TRUE(intersection(ids).empty());
}

TEST_F(VectorUtilsFixture, SumArrayAndResetArrayOperateOnAllElements) {
	std::array<int, 4> values = { 1, 2, 3, 4 };

	EXPECT_EQ(sumArray(values), 10);

	resetArray(values, 5);
	std::array<int, 4> expected = { 5, 5, 5, 5 };
	EXPECT_EQ(values, expected);
}

TEST_F(VectorUtilsFixture, MoveNLastElementsReturnsTailAndShrinksSource) {
	std::vector<int> source = { 1, 2, 3, 4, 5 };
	auto destination = moveNLastElements(source, 2);

	std::vector<int> expectedDestination = { 4, 5 };
	std::vector<int> expectedSource = { 1, 2, 3 };
	EXPECT_EQ(destination, expectedDestination);
	EXPECT_EQ(source, expectedSource);
}

TEST_F(VectorUtilsFixture, MoveNLastElementsClampsOversizedRequest) {
	std::vector<int> source = { 1, 2 };
	auto destination = moveNLastElements(source, 5);

	std::vector<int> expectedDestination = { 1, 2 };
	EXPECT_EQ(destination, expectedDestination);
	EXPECT_TRUE(source.empty());
}

TEST_F(VectorUtilsFixture, CollectSortedBelowOrdersIndexesByValueAscending) {
	std::vector<int> values = { 40, 10, 30, 20 };
	std::vector<unsigned int> out;
	collectSortedBelow(out, std::span<int>(values), 100, -1);

	// All values pass the threshold; indexes returned smallest-value first.
	EXPECT_EQ(out, std::vector<unsigned int>({ 1, 3, 2, 0 }));
}

TEST_F(VectorUtilsFixture, CollectSortedBelowExcludesValuesAboveThreshold) {
	std::vector<int> values = { 40, 10, 30, 20 };
	std::vector<unsigned int> out;
	collectSortedBelow(out, std::span<int>(values), 25, -1);

	// Only values <= 25 (indexes 1 and 3) are kept, still ascending.
	EXPECT_EQ(out, std::vector<unsigned int>({ 1, 3 }));
}

TEST_F(VectorUtilsFixture, CollectSortedBelowKeepsThresholdBoundaryValue) {
	std::vector<int> values = { 40, 10, 30, 20 };
	std::vector<unsigned int> out;
	collectSortedBelow(out, std::span<int>(values), 20, -1);

	// Threshold is inclusive: value == 20 (index 3) is kept.
	EXPECT_EQ(out, std::vector<unsigned int>({ 1, 3 }));
}

TEST_F(VectorUtilsFixture, CollectSortedBelowCapKeepsSmallestValues) {
	std::vector<int> values = { 40, 10, 30, 20, 5 };
	std::vector<unsigned int> out;
	collectSortedBelow(out, std::span<int>(values), 100, 2);

	// All pass the threshold, but only the 2 smallest are kept, ascending.
	EXPECT_EQ(out, std::vector<unsigned int>({ 4, 1 }));
}

TEST_F(VectorUtilsFixture, CollectSortedBelowCapLargerThanQualifyingSetReturnsAll) {
	std::vector<int> values = { 40, 10, 30, 20 };
	std::vector<unsigned int> out;
	collectSortedBelow(out, std::span<int>(values), 25, 10);

	// maxCount exceeds the number of qualifying values: keep them all, ascending.
	EXPECT_EQ(out, std::vector<unsigned int>({ 1, 3 }));
}

TEST_F(VectorUtilsFixture, CollectSortedBelowCapZeroReturnsEmpty) {
	std::vector<int> values = { 40, 10, 30, 20 };
	std::vector<unsigned int> out;
	collectSortedBelow(out, std::span<int>(values), 100, 0);

	EXPECT_TRUE(out.empty());
}

TEST_F(VectorUtilsFixture, CollectSortedBelowExcludesInfinitySentinel) {
	// Mirrors getAreas: unseen cells are marked +inf and must be filtered out.
	constexpr float inf = std::numeric_limits<float>::max();
	std::vector<float> values = { inf, 2.f, inf, 1.f, 3.f };
	std::vector<unsigned int> out;
	collectSortedBelow(out, std::span<float>(values), 100.f, -1);

	EXPECT_EQ(out, std::vector<unsigned int>({ 3, 1, 4 }));
}

TEST_F(VectorUtilsFixture, CollectSortedBelowReusesBufferAcrossCalls) {
	std::vector<int> first = { 4, 1, 3, 2 };
	std::vector<int> second = { 30, 50, 10 };
	std::vector<unsigned int> out = { 99, 98, 97 }; // pre-populated stale contents

	collectSortedBelow(out, std::span<int>(first), 100, -1);
	EXPECT_EQ(out, std::vector<unsigned int>({ 1, 3, 2, 0 }));

	// Second call must clear stale state and not leak entries from the first.
	collectSortedBelow(out, std::span<int>(second), 100, -1);
	EXPECT_EQ(out, std::vector<unsigned int>({ 2, 0, 1 }));
}

TEST_F(VectorUtilsFixture, CollectSortedBelowOutputIsNonDecreasingByValue) {
	std::vector<int> values = { 7, 3, 9, 1, 8, 2, 6, 4 };
	std::vector<unsigned int> out;
	collectSortedBelow(out, std::span<int>(values), 100, 5);

	ASSERT_EQ(out.size(), 5u);
	// Invariant getAreas relies on: result strictly ordered ascending by value.
	for (size_t i = 1; i < out.size(); ++i) {
		EXPECT_LE(values[out[i - 1]], values[out[i]]);
	}
	// And those must be the 5 smallest values: {1,2,3,4,6}.
	EXPECT_EQ(out, std::vector<unsigned int>({ 3, 5, 1, 7, 6 }));
}
