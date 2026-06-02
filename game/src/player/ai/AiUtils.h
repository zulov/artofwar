#pragma once
#include <cmath>
#include <functional>
#include <vector>
#include "math/RandGen.h"
#include "math/VectorUtils.h"
#include "utils/SpanUtils.h"

class Player;
struct db_unit;
struct db_building;
enum class ParentBuildingType : char;

template <typename E>
constexpr unsigned char idx(E e) { return static_cast<unsigned char>(e); }

inline float norm(int value, float scale) { return static_cast<float>(value) / scale; }
inline float norm(unsigned value, float scale) { return static_cast<float>(value) / scale; }
inline float norm(float value, float scale) { return value / scale; }

inline int roundToInt(float value) { return static_cast<int>(std::round(value)); }

// Average level of units matching a filter, normalized per-type by max level. Returns [0, 1].
float avgUnitLevel(const std::vector<db_unit*>& units, Player* player,
                   const std::function<bool(const db_unit*)>& filter);

// Average level of buildings matching a ParentBuildingType, normalized per-type by max level. Returns [0, 1].
float avgBuildingLevel(const std::vector<db_building*>& buildings, Player* player,
                       ParentBuildingType type);

inline bool randFromTwo(float val) {
	return val + 1.f > RandGen::nextRand(RandFloatType::AI, 2.f);
}

inline int sampleWeighted(std::span<const float> weights, float totalWeight) {
	if (totalWeight <= 0) { return 0; }
	const float rand = RandGen::nextRand(RandFloatType::AI, totalWeight);
	float sum = 0.f;
	for (int i = 0; i < weights.size(); ++i) {
		sum += weights[i];
		if (rand < sum) {
			return i;
		}
	}
	return static_cast<int>(weights.size()) - 1;
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
	return sampleWeighted(clamped, max);
}

inline int lowestWithRand(std::span<const float> diffs) {
	assert(allPositive(__LINE__, __FILE__, diffs));
	const float sum = std::accumulate(diffs.begin(), diffs.end(), 0.f);

	std::vector<float> inverted(diffs.begin(), diffs.end());
	for (float& val : inverted) {
		val = sum - val;
	}

	return biggestWithRand(inverted);
}

inline std::vector<int> lowestWithRand(std::span<const float> diffs, int count) {
	assert(allPositive(__LINE__, __FILE__, diffs));
	const float sum = std::accumulate(diffs.begin(), diffs.end(), 0.f);

	std::vector<float> inverted(diffs.begin(), diffs.end());
	for (float& val : inverted) {
		val = sum - val;
		if (val < 0) { val = 0.f; }
	}

	const float totalWeight = std::accumulate(inverted.begin(), inverted.end(), 0.f);
	std::vector<int> result;
	result.reserve(count);
	for (int i = 0; i < count; ++i) {
		result.push_back(sampleWeighted(inverted, totalWeight));
	}
	return result;
}
