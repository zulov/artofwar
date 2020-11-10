#include "ThresholdCalculator.h"

#include "simulation/env/influence/CenterType.h"


float ThresholdCalculator::hasReach(float* threshold, std::span<float> value) {
	float sum = 0;
	for (int i = 0; i < value.size(); ++i) {
		auto d = threshold[i] - value[i];
		if (d > 0.f) {
			sum += d * d;
		}
	}
	return sum;
}

float ThresholdCalculator::diff(float* threshold, std::span<float> value) {
	float sum = 0;
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
	return hasReach(econAttackCenter, value) == 0.f
		|| hasReach(buildingAttackCenter, value) == 0.f
		|| hasReach(unitsAttackCenter, value) == 0.f;
}

CenterType ThresholdCalculator::getBestToAttack(std::span<float> value) {
	float a = diff(econAttackCenter, value);
	float b = diff(buildingAttackCenter, value);
	float c = diff(unitsAttackCenter, value);

	if (a > b && a > c) {
		return CenterType::ECON;
	}
	if (b > c) {
		return CenterType::BUILDING;
	}
	return CenterType::UNITS;
}
