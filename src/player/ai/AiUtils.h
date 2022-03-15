#pragma once
#include "math/RandGen.h"
#include "math/VectorUtils.h"
#include "ActionMakerLogger.h"


static float getBestThree(int ids[3], float vals[3], std::span<float> v) {
	auto sortedIdx = sort_indexes_desc(v);

	for (int i = 0; i < 3; ++i) {
		ids[i] = sortedIdx[i];
		vals[i] = v[sortedIdx[i]];
		if (vals[i] < 0) {
			vals[i] = 0;
		}
	}
	return std::accumulate(vals, vals + 3, 0.f);
}


inline float getLowestThree(int ids[3], float vals[3], std::span<float> v) {
	auto sortedIdx = sort_indexes(v);
	for (int i = 0; i < 3; ++i) {
		ids[i] = sortedIdx[i];
		vals[i] = v[sortedIdx[i]];
	}

	return std::accumulate(vals, vals + 3, 0.f);
}

inline float mirror(float vals[3], float sum) {
	for (int i = 0; i < 3; ++i) {
		vals[i] = sum - vals[i];
	}
	return std::accumulate(vals, vals + 3, 0.f);
}


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
	assert(diffs.size() >= 3);
	int ids[3];
	float vals[3];
	float max = getLowestThree(ids, vals, diffs);

	if (max <= 0) { return ids[0]; }
	max = mirror(vals, max);

	return randFromThree(ids, vals, max);
}

inline int biggestWithRand(std::span<float> vals) {
	assert(!vals.empty());
	for (float& val : vals) {
		if (val < 0) {
			val = 0.f;
		}
	}
	assert(allPositive(__LINE__, __FILE__, vals));
	float max = std::accumulate(vals.begin(), vals.end(), 0.f);
	if (max <= 0) { return 0; }
	float rand = RandGen::nextRand(RandFloatType::AI, max);
	float sum = 0.f;
	for (int i = 0; i < vals.size(); ++i) {
		sum += vals[i];
		if (rand < sum) {
			return i;
		}
	}
}
