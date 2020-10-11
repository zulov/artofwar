#pragma once
#include <magic_enum.hpp>
#include <span>
#include "StatsEnums.h"
#include "database/db_strcut.h"

struct db_basic_metric;

class AiInputProvider {
public:
	AiInputProvider();
	AiInputProvider(const AiInputProvider&) = delete;

	std::span<float> getBasicInput(short id);

	std::span<float> getResourceInput(char playerId);
	std::span<float> getUnitsInput(char playerId);
	std::span<float> getBuildingsInput(char playerId);
	std::span<float> getBasicInputWithMetric(char playerId, const db_basic_metric* prop);
private:

	void update(short id, float* data);

	float basicInput[magic_enum::enum_count<BasicInputType>() * 2];
	std::span<float> basicInputSpan = std::span(basicInput);

	float resourceIdInput[magic_enum::enum_count<BasicInputType>() * 2 + magic_enum::enum_count<ResourceInputType>()];
	std::span<float> resourceIdInputSpan = std::span(resourceIdInput);

	float unitsInput[magic_enum::enum_count<BasicInputType>() * 2 + magic_enum::enum_count<UnitMetric>() * 2];
	std::span<float> unitsInputSpan = std::span(unitsInput);

	float buildingsInput[magic_enum::enum_count<BasicInputType>() * 2 + magic_enum::enum_count<BuildingMetric>() * 2];
	std::span<float> buildingsInputSpan = std::span(buildingsInput);

	float basicWithMetric[magic_enum::enum_count<BasicInputType>() * 2 + magic_enum::enum_count<BuildingMetric>()];
	std::span<float> basicWithMetricBuildingSpan = std::span(basicWithMetric,
	                                                         magic_enum::enum_count<BasicInputType>() * 2 +
	                                                         magic_enum::enum_count<BuildingMetric>());
	std::span<float> basicWithMetricUnitSpan = std::span(basicWithMetric,
	                                                     magic_enum::enum_count<BasicInputType>() * 2 +
	                                                     magic_enum::enum_count<UnitMetric>());

	float wBasic[magic_enum::enum_count<BasicInputType>()];
	float wResourceInput[magic_enum::enum_count<ResourceInputType>()];
	float wUnitsSumInput[magic_enum::enum_count<UnitMetric>() - 1];
	float wBuildingsSumInput[magic_enum::enum_count<BuildingMetric>() - 1];
};
