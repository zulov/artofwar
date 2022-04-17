#pragma once
#include "math/RandGen.h"
#include "math/VectorUtils.h"
#include "ActionMakerLogger.h"

inline bool randFromTwo(float val) {
	return val + 1.f > RandGen::nextRand(RandFloatType::AI, 2.f);
}

inline int biggestWithRand(std::span<float> vals) {
	assert(validateSpan(__LINE__, __FILE__, vals));
	assert(!vals.empty());
	for (float& val : vals) {
		if (val < 0) {
			val = 0.f;
		}
	}
	assert(allPositive(__LINE__, __FILE__, vals));
	const float max = std::accumulate(vals.begin(), vals.end(), 0.f);
	if (max <= 0) { return 0; }
	const float rand = RandGen::nextRand(RandFloatType::AI, max);
	float sum = 0.f;
	for (int i = 0; i < vals.size(); ++i) {
		sum += vals[i];
		if (rand < sum) {
			return i;
		}
	}
}

inline int lowestWithRand(std::span<float> diffs) {
	assert(allPositive(__LINE__, __FILE__, diffs));
	const float sum = std::accumulate(diffs.begin(), diffs.end(), 0.f);

	for (float& val : diffs) {
		val = sum - val;
	}

	return biggestWithRand(diffs);
}
