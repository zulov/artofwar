#pragma once

#include <cassert>
#include <numeric>
#include <span>
#include <vector>

#include "math/RandGen.h"
#include "utils/SpanUtils.h"

struct DecideResult {
	std::span<const float> data;

	DecideResult(std::span<const float> data) : data(data) {
		assert(validateSpan(__LINE__, __FILE__, data));
	}

	operator std::span<const float>() const { return data; }

	float operator[](size_t i) const {
		assert(i < data.size());
		return data[i];
	}
	size_t size() const { return data.size(); }
	bool empty() const { return data.empty(); }
	auto begin() const { return data.begin(); }
	auto end() const { return data.end(); }

	bool doIf() const {
		assert(data.size() == 1);
		return data[0] + 1.f > RandGen::nextRand(RandFloatType::AI, 2.f);
	}

	template <typename E>
	E bestAs() const { return static_cast<E>(best()); }

	int best() const {
		assert(!data.empty());
		assert(validateSpan(__LINE__, __FILE__, data));

		std::vector<float> clamped(data.begin(), data.end());
		for (float& val : clamped) {
			if (val < 0) val = 0.f;
		}
		const float max = std::accumulate(clamped.begin(), clamped.end(), 0.f);
		if (max <= 0) { return 0; }
		const float rand = RandGen::nextRand(RandFloatType::AI, max);
		float sum = 0.f;
		for (int i = 0; i < clamped.size(); ++i) {
			sum += clamped[i];
			if (rand < sum) { return i; }
		}
		return static_cast<int>(clamped.size()) - 1;
	}

	int lowest() const {
		assert(!data.empty());
		assert(allPositive(__LINE__, __FILE__, data));
		const float sum = std::accumulate(data.begin(), data.end(), 0.f);
		std::vector<float> inverted(data.begin(), data.end());
		for (float& val : inverted) {
			val = sum - val;
		}
		return DecideResult(inverted).best();
	}
};
