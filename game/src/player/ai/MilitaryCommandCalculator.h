#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <optional>

#include <Urho3D/Math/Vector2.h>

#include "MilitaryBrain.h"

struct MilitaryCommandScore {
	MilitaryCenterIdx center = MilitaryCenterIdx::OUR_ARMY;
	float score = 0.f;
};

struct MilitaryCommandCalculation {
	MilitaryCommandScore best{};
	std::array<float, MILITARY_CENTER_COUNT> scores{};
};

class MilitaryCommandCalculator {
public:
	explicit MilitaryCommandCalculator(float radius) : radius(radius) {}

	MilitaryCommandCalculation calculate(const Urho3D::Vector2& unitPos,
	                                    const std::array<std::optional<Urho3D::Vector2>, MILITARY_CENTER_COUNT>& centers,
	                                    const MilitaryOutput& output) const {
		MilitaryCommandCalculation result;
		result.scores.fill(0.f);

		std::array<float, MILITARY_CENTER_COUNT> sourceWeights{};
		for (size_t i = 0; i < MILITARY_CENTER_COUNT; ++i) {
			if (!centers[i].has_value()) { continue; }
			const float distance = (unitPos - centers[i].value()).Length();
			sourceWeights[i] = closeness(distance);
		}

		for (size_t source = 0; source < MILITARY_CENTER_COUNT; ++source) {
			if (sourceWeights[source] <= 0.f) { continue; }
			for (size_t target = 0; target < MILITARY_CENTER_COUNT; ++target) {
				if (source == target || !centers[target].has_value()) { continue; }
				result.scores[target] += sourceWeights[source] * output.pressure(
					static_cast<MilitaryCenterIdx>(source), static_cast<MilitaryCenterIdx>(target));
			}
		}

		bool hasBest = false;
		for (size_t i = 0; i < MILITARY_CENTER_COUNT; ++i) {
			if (!centers[i].has_value()) { continue; }
			if (!hasBest || result.scores[i] > result.best.score) {
				result.best = {static_cast<MilitaryCenterIdx>(i), result.scores[i]};
				hasBest = true;
			}
		}

		return result;
	}

	private:
	float closeness(float distance) const {
		if (radius <= 0.f) { return 0.f; }
		return std::clamp(1.f - distance / radius, 0.f, 1.f);
	}

	float radius;
};
