#pragma once
#include <span>
#include <algorithm>
#include <numeric>
#include <iostream>

inline float minSpan(std::span<float> vec) {
	return *std::min(vec.begin(), vec.end());
}

inline float sumSpan(std::span<float> vec) {
	return std::accumulate(vec.begin(), vec.end(), 0.f);
}

inline float minAndSumSpan(std::span<float> vec) {
	return sumSpan(vec) + minSpan(vec);
}

inline void printSpan(std::span<float> vec) {
	for (auto val : vec) {
		std::cout << val << ";";
	}
	std::cout << std::endl;
}

inline void validateSpan(std::span<float> vec) {
	bool valid = true;
	for (auto val : vec) {
		if (isnan(val)) {
			valid = false;
		}
	}
	if(!valid) {
		printSpan(vec);
		assert(false);
	}
}
