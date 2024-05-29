#pragma once
#include <array>

#include "MetricDefinitions.h"
#include "math/VectorUtils.h"
#include "utils/defines.h"

struct PossessionMetric {
	PossessionMetric() {
		resetArray(unitsSumAsSpan);
		resetArray(freeArmySumAsSpan);
		resetArray(buildingsSumAsSpan);

		resetArray(buildingsOtherSumSpan);
		resetArray(buildingsDefenceSumSpan);
		resetArray(buildingsTechSumSpan);
		resetArray(buildingsUnitsSumSpan);
		resetArray(resWithoutBonus);
	}

	std::array<float, UNIT_SIZE> unitsSumAsSpan;
	std::array<float, UNIT_SIZE> freeArmySumAsSpan; //TODO to dac jako któtkie
	std::array<float, BUILDING_SIZE> buildingsSumAsSpan;

	std::array<float, BUILDING_OTHER_SIZE> buildingsOtherSumSpan;
	std::array<float, BUILDING_DEF_SIZE> buildingsDefenceSumSpan;
	std::array<float, BUILDING_TECH_SIZE> buildingsTechSumSpan;
	std::array<float, BUILDING_UNITS_SIZE> buildingsUnitsSumSpan;
	std::array<float, RESOURCES_SIZE> resWithoutBonus;
};
