#pragma once
#include "math/RandGen.h"
#include "math/VectorUtils.h"
#include "ActionMakerLogger.h"

inline bool randFromTwo(float val) {
	return val < RandGen::nextRand(RandFloatType::AI, 1.f);
}

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
