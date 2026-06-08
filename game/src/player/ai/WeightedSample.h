#pragma once
#include <cassert>
#include <span>
#include <vector>

#include "math/RandGen.h"
#include "utils/SpanUtils.h"

// Stochastic samplers shared by the NN output consumers and AiUtils.
// All draws come from the dedicated AI random stream (RandFloatType::AI).
//
// These are allocation-free: the clamped / inverted transforms are applied
// inline (once to compute the total, once during the cumulative walk) instead
// of materializing a temporary vector.
namespace weighted {

// Picks an index with probability proportional to weights[i] (weights assumed >= 0).
// Returns 0 if the total weight is non-positive.
inline int sample(std::span<const float> weights, float totalWeight) {
	if (totalWeight <= 0.f) { return 0; }
	const float rand = RandGen::nextRand(RandFloatType::AI, totalWeight);
	float sum = 0.f;
	for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
		sum += weights[i];
		if (rand < sum) { return i; }
	}
	return static_cast<int>(weights.size()) - 1;
}

// Weighted pick over max(value, 0): higher values are more likely.
inline int biggest(std::span<const float> vals) {
	assert(validateSpan(__LINE__, __FILE__, vals));
	assert(!vals.empty());

	float total = 0.f;
	for (float v : vals) { total += v > 0.f ? v : 0.f; }
	if (total <= 0.f) { return 0; }

	const float rand = RandGen::nextRand(RandFloatType::AI, total);
	float sum = 0.f;
	const int n = static_cast<int>(vals.size());
	for (int i = 0; i < n; ++i) {
		sum += vals[i] > 0.f ? vals[i] : 0.f;
		if (rand < sum) { return i; }
	}
	return n - 1;
}

// Weighted pick over (sum - value): lower values are more likely.
// Inputs must be non-negative.
inline int lowest(std::span<const float> diffs) {
	assert(allPositive(__LINE__, __FILE__, diffs));
	assert(!diffs.empty());

	float sum = 0.f;
	for (float v : diffs) { sum += v; }

	// inverted[i] = max(sum - diffs[i], 0); total = sum of inverted.
	float total = 0.f;
	for (float v : diffs) {
		const float inv = sum - v;
		total += inv > 0.f ? inv : 0.f;
	}
	if (total <= 0.f) { return 0; }

	const float rand = RandGen::nextRand(RandFloatType::AI, total);
	float acc = 0.f;
	const int n = static_cast<int>(diffs.size());
	for (int i = 0; i < n; ++i) {
		const float inv = sum - diffs[i];
		acc += inv > 0.f ? inv : 0.f;
		if (rand < acc) { return i; }
	}
	return n - 1;
}

// Draws `count` independent indices favouring lower values (same distribution as
// lowest()). Materializes the inverted-clamped weights once, then samples count times.
inline std::vector<int> lowestN(std::span<const float> diffs, int count) {
	assert(allPositive(__LINE__, __FILE__, diffs));

	float sum = 0.f;
	for (float v : diffs) { sum += v; }

	std::vector<float> inverted(diffs.size());
	float total = 0.f;
	for (size_t i = 0; i < diffs.size(); ++i) {
		const float inv = sum - diffs[i];
		inverted[i] = inv > 0.f ? inv : 0.f;
		total += inverted[i];
	}

	std::vector<int> result;
	result.reserve(count);
	for (int i = 0; i < count; ++i) {
		result.push_back(sample(inverted, total));
	}
	return result;
}

} // namespace weighted
