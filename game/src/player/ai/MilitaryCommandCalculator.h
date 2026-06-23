#pragma once

#include <algorithm>
#include <array>
#include <cstddef>

#include <Urho3D/Math/Vector2.h>

#include "MilitaryBrain.h"
#include "utils/OtherUtils.h"

struct MilitaryCenterSnapshot {
	bool available = false;
	Urho3D::Vector2 position{};
};

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
	                                    const std::array<MilitaryCenterSnapshot, MILITARY_CENTER_COUNT>& centers,
	                                    const MilitaryOutput& output) const {
		MilitaryCommandCalculation result;
		result.scores.fill(0.f);

		std::array<float, MILITARY_CENTER_COUNT> sourceWeights{};
		for (size_t i = 0; i < MILITARY_CENTER_COUNT; ++i) {
			if (!centers[i].available) { continue; }
			const float distance = (unitPos - centers[i].position).Length();
			sourceWeights[i] = closeness(distance);
		}

		for (size_t source = 0; source < MILITARY_CENTER_COUNT; ++source) {
			if (sourceWeights[source] <= 0.f) { continue; }
			for (size_t target = 0; target < MILITARY_CENTER_COUNT; ++target) {
				if (source == target || !centers[target].available) { continue; }
				result.scores[target] += sourceWeights[source] * output.pressure(
					static_cast<MilitaryCenterIdx>(source), static_cast<MilitaryCenterIdx>(target));
			}
		}

		for (size_t i = 0; i < MILITARY_CENTER_COUNT; ++i) {
			if (!centers[i].available) { continue; }
			if (result.best.score == 0.f && result.best.center == MilitaryCenterIdx::OUR_ARMY && i != 0) {
				result.best = {static_cast<MilitaryCenterIdx>(i), result.scores[i]};
				continue;
			}
			if (result.scores[i] > result.best.score || !centers[castC(result.best.center)].available) {
				result.best = {static_cast<MilitaryCenterIdx>(i), result.scores[i]};
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
