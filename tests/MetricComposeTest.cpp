#include "pch.h"

#include <array>
#include <span>

#include "player/ai/MetricDefinitions.h"

class MetricComposeFixture : public ::testing::Test {};

namespace {

struct TestMetric {
	std::function<float(int*, int*)> fn;
	float weight;
};

template <size_t N>
auto section(const std::array<TestMetric, N>& metrics, int* one, int* two) {
	struct S {
		const std::array<TestMetric, N>& metrics; int* one; int* two;
		int write(float* out) const {
			int i = 0;
			for (auto const& v : metrics) { out[i++] = v.fn(one, two) * v.weight; }
			return i;
		}
	};
	return S{metrics, one, two};
}

} // namespace

TEST_F(MetricComposeFixture, SingleSectionFromMetricArray) {
	std::array<TestMetric, 3> metrics = {{
		{[](int* a, int*) { return static_cast<float>(*a); }, 0.5f},
		{[](int*, int* b) { return static_cast<float>(*b); }, 0.25f},
		{[](int* a, int* b) { return static_cast<float>(*a + *b); }, 1.f},
	}};

	int a = 10, b = 4;
	auto result = METRIC_DEFINITIONS.compose(section(metrics, &a, &b));

	ASSERT_EQ(result.size(), 3u);
	EXPECT_FLOAT_EQ(result[0], 10.f * 0.5f);   // 5.0
	EXPECT_FLOAT_EQ(result[1], 4.f * 0.25f);    // 1.0
	EXPECT_FLOAT_EQ(result[2], 14.f * 1.f);     // 14.0
}

TEST_F(MetricComposeFixture, SingleSectionFromSpan) {
	const std::array<float, 4> data = {1.f, 2.f, 3.f, 4.f};
	auto result = METRIC_DEFINITIONS.compose(
		section(std::span<const float>(data))
	);

	ASSERT_EQ(result.size(), 4u);
	EXPECT_FLOAT_EQ(result[0], 1.f);
	EXPECT_FLOAT_EQ(result[1], 2.f);
	EXPECT_FLOAT_EQ(result[2], 3.f);
	EXPECT_FLOAT_EQ(result[3], 4.f);
}

TEST_F(MetricComposeFixture, SingleSectionFromSpanSelective) {
	const std::array<float, 5> data = {10.f, 20.f, 30.f, 40.f, 50.f};
	const std::array<unsigned char, 3> indices = {1, 3, 4};
	auto result = METRIC_DEFINITIONS.compose(
		section(std::span<const float>(data), std::span<const unsigned char>(indices))
	);

	ASSERT_EQ(result.size(), 3u);
	EXPECT_FLOAT_EQ(result[0], 20.f);
	EXPECT_FLOAT_EQ(result[1], 40.f);
	EXPECT_FLOAT_EQ(result[2], 50.f);
}

TEST_F(MetricComposeFixture, ComposeTwoSections) {
	std::array<TestMetric, 2> metricsA = {{
		{[](int* a, int*) { return static_cast<float>(*a); }, 1.f},
		{[](int*, int* b) { return static_cast<float>(*b); }, 1.f},
	}};
	const std::array<float, 3> spanData = {7.f, 8.f, 9.f};

	int a = 1, b = 2;
	auto result = METRIC_DEFINITIONS.compose(
		section(metricsA, &a, &b),
		section(std::span<const float>(spanData))
	);

	ASSERT_EQ(result.size(), 5u);
	EXPECT_FLOAT_EQ(result[0], 1.f);
	EXPECT_FLOAT_EQ(result[1], 2.f);
	EXPECT_FLOAT_EQ(result[2], 7.f);
	EXPECT_FLOAT_EQ(result[3], 8.f);
	EXPECT_FLOAT_EQ(result[4], 9.f);
}

TEST_F(MetricComposeFixture, ComposeThreeSections) {
	const std::array<float, 2> span1 = {1.f, 2.f};
	const std::array<float, 2> span2 = {3.f, 4.f};
	const std::array<float, 2> span3 = {5.f, 6.f};

	auto result = METRIC_DEFINITIONS.compose(
		section(std::span<const float>(span1)),
		section(std::span<const float>(span2)),
		section(std::span<const float>(span3))
	);

	ASSERT_EQ(result.size(), 6u);
	for (int i = 0; i < 6; ++i) {
		EXPECT_FLOAT_EQ(result[i], static_cast<float>(i + 1));
	}
}

TEST_F(MetricComposeFixture, ComposeEmptySectionsProducesEmpty) {
	const std::array<float, 0> empty = {};
	auto result = METRIC_DEFINITIONS.compose(
		section(std::span<const float>(empty))
	);

	ASSERT_EQ(result.size(), 0u);
}

TEST_F(MetricComposeFixture, ComposeMetricThenSpanThenSelective) {
	std::array<TestMetric, 1> metrics = {{
		{[](int* a, int*) { return static_cast<float>(*a); }, 0.1f},
	}};
	const std::array<float, 3> spanData = {10.f, 20.f, 30.f};
	const std::array<unsigned char, 2> indices = {0, 2};

	int a = 100;
	auto result = METRIC_DEFINITIONS.compose(
		section(metrics, &a, &a),
		section(std::span<const float>(spanData)),
		section(std::span<const float>(spanData), std::span<const unsigned char>(indices))
	);

	ASSERT_EQ(result.size(), 6u);
	EXPECT_FLOAT_EQ(result[0], 100.f * 0.1f);  // metric: 10.0
	EXPECT_FLOAT_EQ(result[1], 10.f);           // span[0]
	EXPECT_FLOAT_EQ(result[2], 20.f);           // span[1]
	EXPECT_FLOAT_EQ(result[3], 30.f);           // span[2]
	EXPECT_FLOAT_EQ(result[4], 10.f);           // selective: span[0]
	EXPECT_FLOAT_EQ(result[5], 30.f);           // selective: span[2]
}

TEST_F(MetricComposeFixture, ComposeOutputOverwritesPrevious) {
	const std::array<float, 3> first = {1.f, 2.f, 3.f};
	auto result1 = METRIC_DEFINITIONS.compose(section(std::span<const float>(first)));
	ASSERT_EQ(result1.size(), 3u);

	const std::array<float, 2> second = {99.f, 88.f};
	auto result2 = METRIC_DEFINITIONS.compose(section(std::span<const float>(second)));
	ASSERT_EQ(result2.size(), 2u);
	EXPECT_FLOAT_EQ(result2[0], 99.f);
	EXPECT_FLOAT_EQ(result2[1], 88.f);
	// result1 is now invalidated (shared buffer) — this is expected behavior
}
