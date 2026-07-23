#pragma once
#include <algorithm>
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

template <typename T>
static void sortAndRemoveDuplicates(std::vector<T>& values) {
	std::ranges::sort(values);
	values.erase(std::ranges::unique(values).begin(), values.end());
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

template <typename T>
static void collectSortedBelow(std::vector<unsigned int>& out, std::span<T> v, T threshold, int maxCount) {
	//assert(validateSpan(v));
	out.clear();
	for (unsigned int i = 0; i < v.size(); ++i) {
		if (v[i] <= threshold) {
			out.push_back(i);
		}
	}
	auto pred = [&v](unsigned int i1, unsigned int i2) { return v[i1] < v[i2]; };
	if (maxCount >= 0 && static_cast<int>(out.size()) > maxCount) {
		std::partial_sort(out.begin(), out.begin() + maxCount, out.end(), pred);
		out.resize(maxCount);
	} else {
		std::sort(out.begin(), out.end(), pred);
	}
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
		common = temp; //TODO optimize, może nie kopiować
		if (temp.empty()) {
			break;
		}
	}
	return common;
}

template <typename T, std::size_t N>
T sumArray(std::array<T, N>& arr) {
	T sum = 0;
	for (const auto& value : arr) {
		sum += value;
	}
	return sum;
}

template <typename T, std::size_t N>
void resetArray(std::array<T, N>& arr, T val = 0.f) {
	std::fill_n(arr.begin(), arr.size(), val);
}


template <typename T>
std::vector<T> moveNLastElements(std::vector<T>& source, int size) {
	size = std::min((int)source.size(), size);
	std::vector<T> destination;
	destination.reserve(size);

	auto it = source.end() - size;

	destination.insert(destination.end(),
		std::make_move_iterator(it),
		std::make_move_iterator(source.end()));

	source.erase(it, source.end());
	return destination;
}
