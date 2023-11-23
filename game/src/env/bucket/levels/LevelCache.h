#pragma once
#include <ranges>
#include <vector>

#include "env/GridCalculator.h"

struct GridCalculator;
constexpr char RES_SEP_DIST = 120;

struct LevelCacheValue {
	LevelCacheValue() = default;

	explicit LevelCacheValue(std::vector<short>* indexes)
		: indexes(indexes) {}

	explicit LevelCacheValue(size_t size) {
		indexes = new std::vector<short>();
		shifts = new std::vector<Urho3D::IntVector2>;
		indexes->reserve(size);
		shifts->reserve(size);
	}

	std::vector<short>* indexes{};
	std::vector<Urho3D::IntVector2>* shifts{}; //if null then safe not to check
	short maxShift = 0;

	void dispose() const {
		delete indexes;
		delete shifts;
	}

	void init(size_t size) {
		indexes = new std::vector<short>();
		shifts = new std::vector<Urho3D::IntVector2>;
		indexes->reserve(size);
		shifts->reserve(size);
	}

	std::ranges::zip_view<std::ranges::ref_view<std::vector<short>>, std::ranges::ref_view<std::vector<
		                      Urho3D::IntVector2>>> asZip() const {
		return std::views::zip(*indexes, *shifts);
	}

	void push(Urho3D::IntVector2 first, short second) const {
		assert(indexes->capacity() > indexes->size());
		indexes->push_back(second);
		shifts->push_back(first);
	}
};

class LevelCache {
public:
	explicit LevelCache(float maxDistance, GridCalculator* calculator);
	~LevelCache();

	unsigned short getResolution() const { return calculator->getResolution(); }
	float getMaxDistance() const { return maxDistance; }

	const LevelCacheValue get(float radius, int center) const;
	const LevelCacheValue get(float radius, const Urho3D::IntVector2& centerCords) const;

private:
	LevelCacheValue getEnvIndexs(float radius, LevelCacheValue& prev,
	                             std::vector<Urho3D::IntVector2>& temp,
	                             std::vector<Urho3D::IntVector2>& tempA,
	                             std::vector<Urho3D::IntVector2>& tempB,
	                             std::vector<Urho3D::IntVector2>& tempC) const;
	float maxDistance;
	float invDiff;

	GridCalculator* calculator;
	LevelCacheValue levels[RES_SEP_DIST];
};
