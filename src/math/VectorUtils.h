#pragma once
#include <numeric>
#include <vector>
#include <span>
#include "objects/unit/Unit.h"
#include "objects/building/Building.h"
#include "objects/resource/ResourceEntity.h"

inline auto isAlive = [](Physical* u) {
	return !u->isAlive();
};

static void cleanDead(std::vector<Unit*>& vector, bool sthDead = true) {
	if (sthDead) {
		vector.erase(
			std::remove_if(vector.begin(), vector.end(), isAlive),
			vector.end());
	}
}

static void cleanDead(std::vector<Building*>& vector, bool sthDead = true) {
	if (sthDead) {
		vector.erase(
			std::remove_if(vector.begin(), vector.end(), isAlive),
			vector.end());
	}
}

static void cleanDead(std::vector<Building*>* vector, bool sthDead = true) {
	if (sthDead) {
		vector->erase(
			std::remove_if(vector->begin(), vector->end(), isAlive),
			vector->end());
	}
}

static void cleanDead(std::vector<ResourceEntity*>& vector, bool sthDead = true) {
	if (sthDead) {
		vector.erase(
			std::remove_if(vector.begin(), vector.end(), isAlive),
			vector.end());
	}
}

static std::vector<short> intersection(std::vector<std::vector<short>*>& ids) {
	std::vector<short> common; //TODO check if ids sorted sometimes?
	if (ids.empty()) {
		return common;
	}
	common = *ids[0];
	for (int i = 1; i < ids.size(); ++i) {
		std::vector<short> temp;
		std::set_intersection(common.begin(), common.end(),
		                      ids[i]->begin(), ids[i]->end(),
		                      std::back_inserter(temp));
		common = temp; //TODO optimize, mo¿e nie kopiowaæ
		if (temp.empty()) {
			break;
		}
	}
	return common;
}

static float getBestThree(int ids[3], float vals[3], std::span<float> v) {
	for (int i = 0; i < 3; ++i) {
		const auto max = std::max_element(v.begin(), v.end());
		ids[i] = max - v.begin();
		vals[i] = *max;
		*max = -100;
	}

	for (int i = 0; i < 3; ++i) {
		if (vals[i] < 0) {
			vals[i] = 0;
		}
	}
	return std::accumulate(vals, vals + 3, 0.f);
}

static float getLowestThree(int ids[3], float vals[3], std::vector<float> v) {
	for (int i = 0; i < 3; ++i) {
		const auto min = std::min_element(v.begin(), v.end());
		ids[i] = min - v.begin();
		vals[i] = *min;
		*min = 100000;
	}

	return std::accumulate(vals, vals + 3, 0.f);
}

static float mirror(float vals[3], float sum) {
	for (int i = 0; i < 3; ++i) {
		vals[i] = sum - vals[i];
	}
	return std::accumulate(vals, vals + 3, 0.f);
}

template <typename T>
static std::vector<unsigned int> sort_indexes_desc(T v[], int size) {
	std::vector<unsigned int> idx(size);

	std::iota(idx.begin(), idx.end(), 0);

	stable_sort(idx.begin(), idx.end(),
	            [&v](auto i1, auto i2) { return v[i1] > v[i2]; });

	return idx;
}

template <typename T>
static std::vector<unsigned int> sort_indexes(T v[], int size) {
	std::vector<unsigned int> idx(size);

	std::iota(idx.begin(), idx.end(), 0);

	stable_sort(idx.begin(), idx.end(),
	            [&v](auto i1, auto i2) { return v[i1] < v[i2]; });

	return idx;
}
