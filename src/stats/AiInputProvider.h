#pragma once
#include <magic_enum.hpp>
#include <span>
#include "StatsEnums.h"
#include "objects/Metrics.h"

struct db_unit_metric;
struct db_building_metric;
struct db_basic_metric;

class AiInputProvider {
public:
	AiInputProvider();
	AiInputProvider(const AiInputProvider&) = delete;

	std::span<float> getBasicInput(short id);

	std::span<float> getResourceInput(char playerId);
	std::span<float> getUnitsInput(char playerId);
	std::span<float> getBuildingsInput(char playerId);

	std::span<float> getUnitsInputWithMetric(char playerId, const db_unit_metric* prop);
	std::span<float> getBuildingsInputWithMetric(char playerId, const db_building_metric* prop);
private:

	void update(short id, float* data);

	float basicInput[magic_enum::enum_count<BasicInputType>() * 2];
	std::span<float> basicInputSpan = std::span(basicInput);

	float resourceIdInput[magic_enum::enum_count<BasicInputType>() * 2 + magic_enum::enum_count<ResourceInputType>()];
	std::span<float> resourceIdInputSpan = std::span(resourceIdInput);

	float unitsInput[magic_enum::enum_count<BasicInputType>() * 2 + magic_enum::enum_count<UnitMetric>() - 1];
	//bez cost
	std::span<float> unitsInputSpan = std::span(unitsInput);

	float buildingsInput[magic_enum::enum_count<BasicInputType>() * 2 + magic_enum::enum_count<BuildingMetric>() - 1];
	//bez cost
	std::span<float> buildingsInputSpan = std::span(buildingsInput);

	float unitsWithMetric[magic_enum::enum_count<BasicInputType>() * 2 + magic_enum::enum_count<UnitMetric>() * 2 - 1];
	float buildingsWithMetric[magic_enum::enum_count<BasicInputType>() * 2
		+ magic_enum::enum_count<BuildingMetric>() * 2 - 1];

	std::span<float> unitsWithMetricUnitSpan = std::span(unitsWithMetric);
	std::span<float> basicWithMetricUnitSpan = std::span(buildingsWithMetric);

	float wBasic[magic_enum::enum_count<BasicInputType>()];
	float wResourceInput[magic_enum::enum_count<ResourceInputType>()];
	float wUnitsSumInput[magic_enum::enum_count<UnitMetric>() - 1];
	float wBuildingsSumInput[magic_enum::enum_count<BuildingMetric>() - 1];
};
