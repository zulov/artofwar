#pragma once

#include <array>
#include <cassert>
#include <span>

namespace EconomicCenterUtils {
	template <std::size_t ResourceCount>
	void sumRawValues(std::span<float> result, const std::array<std::span<const float>, ResourceCount>& rawValues) {
		for (const auto values : rawValues) {
			assert(values.size() == result.size());
		}

		for (unsigned i = 0; i < result.size(); ++i) {
			result[i] = 0.f;
			for (const auto values : rawValues) {
				result[i] += values[i];
			}
		}
	}
}
