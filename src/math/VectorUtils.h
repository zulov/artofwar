#pragma once
#include <iterator>
#include <numeric>
#include <vector>
#include <span>
#include "objects/Physical.h"

inline auto isAlivePred = [](Physical* physical) {
	return physical == nullptr || !physical->isAlive();
};

template <typename T>
static void cleanDead(std::vector<T*>& vector, bool sthDead = true) {
	if (sthDead) {
		vector.erase(
			std::remove_if(vector.begin(), vector.end(), isAlivePred),
			vector.end());
	}
}

template <typename T>
static void cleanDead(std::vector<T*>* vector, bool sthDead = true) {
	if (sthDead) {
		vector->erase(
			std::remove_if(vector->begin(), vector->end(), isAlivePred),
			vector->end());
	}
}

template <typename T, class _Pr>
static std::vector<unsigned int> sort_indexes(std::span<T> v, _Pr pred) {
	std::vector<unsigned int> idx(v.size());

	std::iota(idx.begin(), idx.end(), 0);

	stable_sort(idx.begin(), idx.end(), pred);
	return idx;
}

template <typename T>
static std::vector<unsigned int> sort_indexes(std::span<T> v) {
	return sort_indexes(v, [&v](auto i1, auto i2) { return v[i1] < v[i2]; });
}

template <typename T>
static std::vector<unsigned int> sort_indexes_desc(std::span<T> v) {
	return sort_indexes(v, [&v](auto i1, auto i2) { return v[i1] > v[i2]; });
}

static std::vector<unsigned char> intersection(std::vector<std::vector<unsigned char>*>& ids) {
	std::vector<unsigned char> common; //TODO check if ids sorted sometimes?
	if (ids.empty()) {
		return common;
	}
	common = *ids[0];
	for (int i = 1; i < ids.size(); ++i) {
		std::vector<unsigned char> temp;
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

static float getLowestThree(int ids[3], float vals[3], std::span<float> v) {
	auto sortedIdx = sort_indexes(v);
	for (int i = 0; i < 3; ++i) {
		ids[i] = sortedIdx[i];
		vals[i] = v[sortedIdx[i]];
	}

	return std::accumulate(vals, vals + 3, 0.f);
}

static float mirror(float vals[3], float sum) {
	for (int i = 0; i < 3; ++i) {
		vals[i] = sum - vals[i];
	}
	return std::accumulate(vals, vals + 3, 0.f);
}
