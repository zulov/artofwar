#pragma once
#include <span>
#include <algorithm>
#include <numeric>
#include <iostream>

inline float minSpan(std::span<float> vec) {
	return *std::min_element(vec.begin(), vec.end());
}

template <typename T>
inline float sumSpan(std::span<T> vec) {
	return std::accumulate(vec.begin(), vec.end(), 0.f);
}

inline float minAndSumSpan(std::span<float> vec) {
	return sumSpan(vec) + minSpan(vec);
}

inline float minSpanSq(std::span<float> vec) {
	const auto a = minSpan(vec);
	return a * a;
}

template <typename T>
inline void printSpan(std::span<T> vec) {
	for (auto val : vec) {
		std::cerr << val << ";";
	}
	std::cerr << std::endl;
}

template <typename T>
inline void printSpan(std::span<T> vec, int cols) {
	int i = 0;
	for (auto val : vec) {
		if (i % cols == 0) {
			std::cerr << std::endl;
		}
		++i;
		std::cerr << val << ";";
	}
	std::cerr << std::endl;
}

inline void validateSpan(int line, std::string file, std::span<float> vec) {
	bool valid = true;
	for (auto val : vec) {
		if (isnan(val) || isinf(val)) {
			valid = false;
		}
	}
	if (!valid) {
		std::cerr << line << " at " << file << std::endl;
		printSpan(vec);
		assert(false);
	}
}

inline float maxSpan(std::span<float> vec) {
	return *std::max_element(vec.begin(), vec.end());
}

inline float maxSpanRoot(std::span<float> vec) {
	validateSpan(__LINE__, __FILE__, vec);
	return sqrt(maxSpan(vec) + 1);
}

inline void resetSpan(std::span<float> vec, float val = 0.f) {
	std::fill_n(vec.begin(), vec.size(), val);
}
