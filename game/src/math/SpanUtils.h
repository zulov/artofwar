#pragma once
#include <span>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <functional>

inline float minSpan(std::span<float> vec) {
	return *std::ranges::min_element(vec);
}

template <typename T>
T sumSpan(std::span<T> vec) {
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
void printSpan(std::span<T> vec) {
	for (auto val : vec) {
		std::cerr << val << ";";
	}
	std::cerr << std::endl;
}

template <typename T>
void printSpanInt(std::span<T> vec) {
	for (auto val : vec) {
		std::cerr << (int)val << ";";
	}
	std::cerr << std::endl;
}

template <typename T>
void printSpan(std::span<T> vec, std::function<bool(T)> condition) {
	for (auto val : vec) {
		if (condition(val)) {
			std::cerr << val << ";";
		}
	}
	std::cerr << std::endl;
}

template <typename T>
void printSpan(std::span<T> vec, int cols) {
	int i = 0;
	for (auto val : vec) {
		if (i % cols == 0) {
			std::cerr << std::endl;
		}
		++i;
		std::cerr << (int)val << ";";
	}
	std::cerr << std::endl;
}

inline bool validateSpan(int line, std::string file, std::span<const float> vec) {
	bool valid = true;
	for (const auto val : vec) {
		if (isnan(val) || isinf(val)) {
			valid = false;
		}
	}
	if (!valid) {
		std::cerr << line << " at " << file << std::endl;
		printSpan(vec);
		assert(false);
	}
	return valid;
}

inline bool allPositive(int line, std::string file, std::span<float> vec) {
	bool valid = true;
	for (const auto val : vec) {
		if (val < 0.f) {
			valid = false;
		}
	}
	if (!valid) {
		std::cerr << line << " at " << file << std::endl;
		printSpan(vec);
		assert(false);
	}
	return valid;
}

inline bool validateSpan(int line, std::string file, std::vector<float> vec) {
	return validateSpan(line, file, std::span(vec.begin(), vec.size()));
}

inline bool zerosSpan(float* start, int size) {
	auto vec = std::span(start, size);
	for (const auto val : vec) {
		if (val != 0.f) {
			assert(false);
		}
	}
	return true;
}

inline float maxSpan(std::span<float> vec) {
	return *std::ranges::max_element(vec);
}

inline float maxSpanRoot(std::span<float> vec) {
	assert(validateSpan(__LINE__, __FILE__, vec));
	return sqrt(maxSpan(vec) + 1);
}

inline void resetSpan(std::span<float> vec, float val = 0.f) {
	std::fill_n(vec.begin(), vec.size(), val);
}

template <typename T, std::size_t N>
const std::span<const T> asSpan(const std::array<T, N>& arr) {
	return std::span(arr.data(), arr.size());
}
