#include "ThresholdCalculator.h"

#include "simulation/env/influence/CenterType.h"

constexpr float THRESHOLD_VAL = 1.f;

bool ThresholdCalculator::hasReach(float* threshold, std::span<float> value) const {
	for (int i = 0; i < value.size(); ++i) {
		auto d = threshold[i] - value[i];
		if (d > THRESHOLD_VAL) {
			return false;
		}
	}
	return true;
}

float ThresholdCalculator::diff(float* threshold, std::span<float> value) const {
	float sum = 0.f;
	for (int i = 0; i < value.size(); ++i) {
		auto d = threshold[i] - value[i];
		if (d > 0.f) {
			sum += d * d;
		} else {
			sum -= d;
		}
	}
	return sum;
}

bool ThresholdCalculator::ifAttack(std::span<float> value) {
	assert((*(&econAttackCenter + 1) - econAttackCenter)==value.size());
	assert((*(&buildingAttackCenter + 1) - buildingAttackCenter)==value.size());
	assert((*(&unitsAttackCenter + 1) - unitsAttackCenter)==value.size());
	return hasReach(econAttackCenter, value)
		|| hasReach(buildingAttackCenter, value)
		|| hasReach(unitsAttackCenter, value);
}

CenterType ThresholdCalculator::getBestToAttack(std::span<float> value) {
	float a = diff(econAttackCenter, value);
	float b = diff(buildingAttackCenter, value);
	float u = diff(unitsAttackCenter, value);

	if (a < b && a < u) {
		return CenterType::ECON;
	}
	if (b < u) {
		return CenterType::BUILDING;
	}
	return CenterType::UNITS;
}
