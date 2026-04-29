#include "pch.h"

#include <array>
#include <span>

#include "math/VectorUtils.h"

class VectorUtilsFixture : public ::testing::Test {};

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
