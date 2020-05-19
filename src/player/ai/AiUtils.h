#pragma once
#include "math/RandGen.h"
#include "math/VectorUtils.h"

inline int randFromThree(std::vector<float> diffs) {
	int ids[3];
	float vals[3];
	float sum = getLowestThree(ids, vals, diffs);

	if (sum <= 0) { return ids[0]; }
	sum = mirror(vals, sum);
	const float rand = RandGen::nextRand(RandFloatType::AI, sum);
	float sumSoFar = 0;

	for (int i = 0; i < 3; ++i) {
		sumSoFar += vals[i];
		if (rand <= sumSoFar) {
			std::cout << "\t" << vals[i] / sum << "%|";
			return ids[i];
		}
	}
	return -1;
}