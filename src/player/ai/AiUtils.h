#pragma once
#include "math/RandGen.h"
#include "math/VectorUtils.h"
#include "ActionMakerLogger.h"

inline int randFromThree(int ids[3], float vals[3], float max) {
	float rand = RandGen::nextRand(RandFloatType::AI, max);
	float sumSoFar = 0;

	for (int i = 0; i < 3; ++i) {
		sumSoFar += vals[i];
		if (rand <= sumSoFar) {
			return ids[i];
		}
	}
	assert(1==0);
	return ids[0];
}

inline int lowestWithRand(std::span<float> diffs) {
	assert(diffs.size()>0);
	int ids[3];
	float vals[3];
	float max = getLowestThree(ids, vals, diffs);

	if (max <= 0) { return ids[0]; }
	max = mirror(vals, max);

	return randFromThree(ids, vals, max);
}


inline int biggestWithRand(std::span<float> v) {
	assert(v.size()>0);
	int ids[3];
	float vals[3];
	float max = getBestThree(ids, vals, v);

	if (max <= 0) { return ids[0]; }

	logThree(ids, vals, max);

	return randFromThree(ids, vals, max);
}

inline std::span<float> copyTo(std::span<float> dest, std::span<float> src1, std::span<float> src2,
                               std::span<float> src3, std::span<float> src4) {
	assert(dest.size() >= src1.size() + src2.size() + src3.size() + src4.size());
	std::ranges::copy(src1, dest.begin());
	std::ranges::copy(src2, dest.begin() + src1.size());
	std::ranges::copy(src3, dest.begin() + src1.size() + src2.size());
	std::ranges::copy(src4, dest.begin() + src1.size() + src2.size() + src3.size());
	assert(validateSpan(__LINE__, __FILE__, dest));

	return dest;
}

inline std::span<float> copyTo(std::span<float> dest, std::span<float> src1, std::span<float> src2,
                               std::span<float> src3) {
	return copyTo(dest, src1, src2, src3, std::span<float>{});
}

inline std::span<float> copyTo(std::span<float> dest, std::span<float> src1, std::span<float> src2) {
	return copyTo(dest, src1, src2, std::span<float>{});
}
