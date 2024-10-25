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

		resetArray(buildingsOther);
		resetArray(buildingsDefence);
		resetArray(buildingsTech);
		resetArray(buildingsUnits);
		resetArray(resWithoutBonus);
	}

	std::array<float, UNIT_SIZE> unitsSum;
	std::array<float, UNIT_SIZE> freeArmySum; //TODO to dac jako któtkie
	std::array<float, BUILDING_SIZE> buildingsSum;

	std::array<float, BUILDING_OTHER_SIZE> buildingsOther;
	std::array<float, BUILDING_DEF_SIZE> buildingsDefence;
	std::array<float, BUILDING_TECH_SIZE> buildingsTech;
	std::array<float, BUILDING_UNITS_SIZE> buildingsUnits;
	std::array<float, RESOURCES_SIZE> resWithoutBonus;
};
