#pragma once
#include <array>

#include "MetricDefinitions.h"
#include "math/VectorUtils.h"
#include "simulation/SimGlobals.h"

struct PossessionMetric {
	PossessionMetric() {
		resetArray(unitsSum);
		resetArray(buildingsSum);

		resetArray(resWithoutBonus);
	}

	std::array<float, UNIT_METRIC_SIZE> unitsSum;
	std::array<float, BUILDING_METRIC_SIZE> buildingsSum;

	std::array<float, RESOURCES_SIZE> resWithoutBonus;
};
