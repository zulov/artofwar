#pragma once
#include <span>
#include <algorithm>

inline float minSpan(std::span<float> vec) {
	return *std::min(vec.begin(), vec.end());
}

inline float sumSpan(std::span<float> vec) {
	return std::accumulate(vec.begin(), vec.end(), 0.f);
}

inline float minAndSumSpan(std::span<float> vec) {
	return sumSpan(vec) + minSpan(vec);
}
