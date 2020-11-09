#include "ThresholdCalculator.h"


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
	return hasReach(econAttackCenter, value) == 0.f
		|| hasReach(buildingAttackCenter, value) == 0.f
		|| hasReach(unitsAttackCenter, value) == 0.f;
}

char ThresholdCalculator::getBestToAttack(std::span<float> value) {
	float a = diff(econAttackCenter, value);
	float b = diff(buildingAttackCenter, value);
	float c = diff(unitsAttackCenter, value);

	if (a > b && a > c) {
		return 1;
	}
	if (b > c) {
		return 2;
	}
	return 3;
}
