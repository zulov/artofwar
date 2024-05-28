#pragma once
#include <array>

#include "MetricDefinitions.h"
#include "utils/defines.h"

struct PossessionMetric {

	std::array<float, UNIT_SIZE> unitsSumAsSpan;//zrobic z tego array
	std::array<float, UNIT_SIZE> freeArmySumAsSpan; //TODO to dac jako któtkie
	std::array<float, BUILDING_SIZE> buildingsSumAsSpan;

	std::array<float, BUILDING_OTHER_SIZE> buildingsOtherSumSpan;
	std::array<float, BUILDING_DEF_SIZE> buildingsDefenceSumSpan;
	std::array<float, BUILDING_TECH_SIZE> buildingsTechSumSpan;
	std::array<float, BUILDING_UNITS_SIZE> buildingsUnitsSumSpan;
	std::array<float, RESOURCES_SIZE> resWithoutBonus;
};
