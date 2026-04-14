#pragma once
#include <vector>
#include "math/RandGen.h"
#include "math/VectorUtils.h"
#include "ActionMakerLogger.h"

inline bool randFromTwo(float val) {
	return val + 1.f > RandGen::nextRand(RandFloatType::AI, 2.f);
}

inline int biggestWithRand(std::span<const float> vals) {
	assert(validateSpan(__LINE__, __FILE__, vals));
	assert(!vals.empty());

	std::vector<float> clamped(vals.begin(), vals.end());
	for (float& val : clamped) {
		if (val < 0) {
			val = 0.f;
		}
	}
	assert(allPositive(__LINE__, __FILE__, clamped));
	const float max = std::accumulate(clamped.begin(), clamped.end(), 0.f);
	if (max <= 0) { return 0; }
	const float rand = RandGen::nextRand(RandFloatType::AI, max);
	float sum = 0.f;
	for (int i = 0; i < clamped.size(); ++i) {
		sum += clamped[i];
		if (rand < sum) {
			return i;
		}
	}
	return static_cast<int>(clamped.size()) - 1;
}

inline int lowestWithRand(std::span<const float> diffs) {
	assert(allPositive(__LINE__, __FILE__, diffs));
	const float sum = std::accumulate(diffs.begin(), diffs.end(), 0.f);

	// Invert values in a local copy: lower original values get higher weights
	std::vector<float> inverted(diffs.begin(), diffs.end());
	for (float& val : inverted) {
		val = sum - val;
	}

	return biggestWithRand(inverted);
}
