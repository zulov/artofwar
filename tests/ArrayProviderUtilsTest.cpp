#include "pch.h"

#include <algorithm>

#include "env/bucket/ArrayProviderUtils.h"
#include "env/path/PathCache.h"

namespace {
struct ResettableValue {
	int value = 0;
	int resetCount = 0;

	void resetForReuse() {
		value = 0;
		++resetCount;
	}
};
}

class ArrayProviderUtilsFixture : public ::testing::Test {};

TEST_F(ArrayProviderUtilsFixture, ArrayProviderReusesReleasedArraysOfSameSize) {
	auto* data = ArrayProvider<ResettableValue>::get(16);
	data[0].value = 17;
	data[1].value = 42;

	ArrayProvider<ResettableValue>::release(data, 16);

	auto* reused = ArrayProvider<ResettableValue>::get(16);
	EXPECT_EQ(reused, data);
	EXPECT_EQ(reused[0].value, 0);
	EXPECT_EQ(reused[1].value, 0);
	EXPECT_EQ(reused[0].resetCount, 1);
	EXPECT_EQ(reused[1].resetCount, 1);

	delete[] reused;
}

TEST_F(ArrayProviderUtilsFixture, ArrayProviderKeepsPoolsSeparatedBySize) {
	auto* sizeSixteen = ArrayProvider<ResettableValue>::get(16);
	ArrayProvider<ResettableValue>::release(sizeSixteen, 16);

	auto* sizeSeventeen = ArrayProvider<ResettableValue>::get(17);
	auto* reusedSixteen = ArrayProvider<ResettableValue>::get(16);

	EXPECT_NE(sizeSeventeen, sizeSixteen);
	EXPECT_EQ(reusedSixteen, sizeSixteen);

	delete[] sizeSeventeen;
	delete[] reusedSixteen;
}

TEST_F(ArrayProviderUtilsFixture, ArrayProviderSkipsPoolingBelowMinimumSize) {
	auto* data = ArrayProvider<ResettableValue>::get(15);
	data[0].value = 17;

	ArrayProvider<ResettableValue>::release(data, 15);

	auto* fresh = ArrayProvider<ResettableValue>::get(15);
	EXPECT_EQ(fresh[0].value, 0);
	EXPECT_EQ(fresh[0].resetCount, 0);

	delete[] fresh;
}

TEST_F(ArrayProviderUtilsFixture, PrimitiveArrayProviderFillsFreshArraysWithDefaultValue) {
	auto* data = PrimitiveArrayProvider<int>::get(6, 9);

	for (int i = 0; i < 6; ++i) {
		EXPECT_EQ(data[i], 9);
	}

	delete[] data;
}

TEST_F(ArrayProviderUtilsFixture, PrimitiveArrayProviderReusesAndRefillsReleasedArrays) {
	auto* data = PrimitiveArrayProvider<int>::get(16, 1);
	data[0] = 50;
	data[1] = 60;

	PrimitiveArrayProvider<int>::release(data, 16);

	auto* reused = PrimitiveArrayProvider<int>::get(16, 7);
	EXPECT_EQ(reused, data);

	for (int i = 0; i < 16; ++i) {
		EXPECT_EQ(reused[i], 7);
	}

	delete[] reused;
}

TEST_F(ArrayProviderUtilsFixture, PrimitiveFloatArrayProviderReusesAndRefillsReleasedArrays) {
	auto* data = PrimitiveArrayProvider<float>::get(16, 1.5f);
	PrimitiveArrayProvider<float>::release(data, 16);

	auto* reused = PrimitiveArrayProvider<float>::get(16, -2.25f);
	EXPECT_EQ(reused, data);

	for (int i = 0; i < 16; ++i) {
		EXPECT_FLOAT_EQ(reused[i], -2.25f);
	}

	delete[] reused;
}

TEST_F(ArrayProviderUtilsFixture, PrimitiveArrayProviderCanReuseWithoutRefilling) {
	auto* data = PrimitiveArrayProvider<int>::get(37, 11);
	PrimitiveArrayProvider<int>::release(data, 37);

	auto* reused = PrimitiveArrayProvider<int>::get(37);
	EXPECT_EQ(reused, data);
	EXPECT_EQ(reused[0], 11);
	EXPECT_EQ(reused[36], 11);

	delete[] reused;
}

TEST_F(ArrayProviderUtilsFixture, PrimitiveArrayProviderSkipsPoolingBelowMinimumSize) {
	auto* data = PrimitiveArrayProvider<ResettableValue>::get(15);
	data[0].value = 17;

	PrimitiveArrayProvider<ResettableValue>::release(data, 15);

	auto* fresh = PrimitiveArrayProvider<ResettableValue>::get(15);
	EXPECT_EQ(fresh[0].value, 0);
	EXPECT_EQ(fresh[0].resetCount, 0);

	delete[] fresh;
}

TEST_F(ArrayProviderUtilsFixture, ArrayProviderResetsPathCacheArraysBeforeReuse) {
	auto* data = ArrayProvider<PathCache>::get(16);
	std::vector<int> path = {1, 2, 3};
	data[0].set(4, 5, &path);

	ArrayProvider<PathCache>::release(data, 16);

	auto* reused = ArrayProvider<PathCache>::get(16);
	EXPECT_EQ(reused, data);
	EXPECT_EQ(reused[0].start, -1);
	EXPECT_EQ(reused[0].end, -1);
	EXPECT_TRUE(reused[0].path.empty());

	delete[] reused;
}
