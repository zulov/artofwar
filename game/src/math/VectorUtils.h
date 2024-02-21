#pragma once
#include <iterator>
#include <numeric>
#include <vector>
#include <span>
#include "objects/Physical.h"


inline auto notAlive = [](const Physical* physical) {
	return physical == nullptr || !physical->isAlive();
};

inline auto isNotToDispose = [](const Physical* p) { return !p->isToDispose(); };


inline auto dispose = [](const Physical* p) {
	const auto flag= p->isToDispose();
	if(flag) {
		delete p;
	}
	return flag;
};

template <typename T>
static void cleanDead(std::vector<T*>& vector, bool sthDead = true) {
	if (sthDead) {
		vector.erase(
			std::remove_if(vector.begin(), vector.end(), notAlive),
			vector.end());
	}
}

template <typename T>
static void cleanAndDispose(std::vector<T*>* vector, bool sthDead = true) {
	if (sthDead) {
		vector->erase(
			std::remove_if(vector->begin(), vector->end(), dispose),
			vector->end());
	}
}

template <typename T>
static void cleanDead(std::vector<T*>* vector, bool sthDead = true) {
	if (sthDead) {
		vector->erase(
			std::remove_if(vector->begin(), vector->end(), notAlive),
			vector->end());
	}
}

template <typename T, class _Pr>
static std::vector<unsigned int> sort_indexes(std::span<T> v, _Pr pred, int size) {
	std::vector<unsigned int> idx(v.size());

	std::iota(idx.begin(), idx.end(), 0);

	//stable_sort(idx.begin(), idx.end(), pred);
	if (size != -1) {
		std::partial_sort(idx.begin(), idx.begin() + size, idx.end(), pred);
		idx.resize(size);
	} else {
		stable_sort(idx.begin(), idx.end(), pred);
	}


	return idx;
}

template <typename T>
static std::vector<unsigned int> sort_indexes(std::span<T> v, int size = -1) {
	return sort_indexes(v, [&v](auto i1, auto i2) { return v[i1] < v[i2]; }, size);
}

template <typename T>
static std::vector<unsigned int> sort_indexes_desc(std::span<T> v, int size = -1) {
	return sort_indexes(v, [&v](auto i1, auto i2) { return v[i1] > v[i2]; }, size);
}

static std::vector<unsigned char> intersection(std::vector<std::vector<unsigned char>*>& ids) {
	std::vector<unsigned char> common; //TODO check if ids sorted sometimes?
	if (ids.empty()) {
		return common;
	}
	common = *ids[0];
	for (int i = 1; i < ids.size(); ++i) {
		std::vector<unsigned char> temp;
		std::ranges::set_intersection(common, *ids[i],
		                              std::back_inserter(temp));
		common = temp; //TODO optimize, mo¿e nie kopiowaæ
		if (temp.empty()) {
			break;
		}
	}
	return common;
}
