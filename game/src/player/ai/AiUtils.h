#pragma once
#include <cmath>
#include <functional>
#include <vector>
#include "WeightedSample.h"
#include "math/RandGen.h"
#include "math/VectorUtils.h"
#include "utils/SpanUtils.h"

class Player;
struct db_unit;
struct db_building;
struct db_building_level;
enum class ParentBuildingType : char;
enum class BuildPlacementClass : unsigned char;

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

// Overall tech progress: pooled average upgrade level across ALL units and ALL buildings,
// each normalized per-type by its max level. Returns [0, 1].
float avgTechLevel(const std::vector<db_unit*>& units, const std::vector<db_building*>& buildings, Player* player);

// Placement intent for a building at a given level. Resource subtypes are split so the
// build-spatial brain can aim each at a different area. Precedence when a building matches
// several: RES_BONUS > RES_SPAWNER > RES_CONVERT > RES_REFINE > RES_STORAGE, then the
// ParentBuildingType (first set flag) maps to OTHER/DEFENCE/TECH/UNITS.
BuildPlacementClass classifyPlacement(const db_building* building, const db_building_level* level);

inline bool randFromTwo(float val) {
	return val + 1.f > RandGen::nextRand(RandFloatType::AI, 2.f);
}

inline int sampleWeighted(std::span<const float> weights, float totalWeight) {
	return weighted::sample(weights, totalWeight);
}

inline int biggestWithRand(std::span<const float> vals) {
	return weighted::biggest(vals);
}

inline int lowestWithRand(std::span<const float> diffs) {
	return weighted::lowest(diffs);
}

inline std::vector<int> lowestWithRand(std::span<const float> diffs, int count) {
	return weighted::lowestN(diffs, count);
}
