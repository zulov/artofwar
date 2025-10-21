#pragma once
#include <array>

#include "MetricDefinitions.h"
#include "math/VectorUtils.h"
#include "utils/defines.h"

struct PossessionMetric {
	PossessionMetric() {
		resetArray(unitsSum);
		resetArray(freeArmySum);
		resetArray(buildingsSum);

		resetArray(resWithoutBonus);
	}

	std::array<float, UNIT_METRIC_SIZE> unitsSum;
	std::array<float, UNIT_METRIC_SIZE> freeArmySum; //TODO to dac jako któtkie
	std::array<float, BUILDING_METRIC_SIZE> buildingsSum;

	std::array<float, RESOURCES_SIZE> resWithoutBonus;
};
