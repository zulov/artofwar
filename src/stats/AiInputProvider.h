#pragma once
#include <span>
#include "database/db_strcut.h"
#include "player/ai/AiMetric.h"

#define BASIC_SIZE std::size(METRIC_DEFINITIONS.aiBasicMetric)
#define UNIT_SIZE std::size(METRIC_DEFINITIONS.aiUnitMetric)
#define BUILDING_SIZE std::size(METRIC_DEFINITIONS.aiBuildingMetric)
#define SMALL_UNIT_SIZE std::size(METRIC_DEFINITIONS.aiSmallUnitMetric)
#define SMALL_BUILDING_SIZE std::size(METRIC_DEFINITIONS.aiSmallBuildingMetric)

class Player;
struct db_unit_metric;
struct db_building_metric;
struct db_basic_metric;

class AiInputProvider {
public:
	AiInputProvider() = default;
	AiInputProvider(const AiInputProvider&) = delete;

	std::span<float> getResourceInput(char playerId);
	std::span<float> getUnitsInput(char playerId);
	std::span<float> getBuildingsInput(char playerId);

	std::span<float> getUnitsInputWithMetric(char playerId, const db_unit_metric* prop);
	std::span<float> getBuildingsInputWithMetric(char playerId, const db_building_metric* prop);
private:
	//TODO zastapic spany std::array
	std::span<float> getBasicInput(std::span<float> dest, Player* player);
	void applyWeights(std::span<float> dest, float* weights, size_t skip);

	float resourceIdInput[BASIC_SIZE + std::size(METRIC_DEFINITIONS.aiResourceMetric)];
	float unitsInput[BASIC_SIZE + UNIT_SIZE];
	float buildingsInput[BASIC_SIZE + BUILDING_SIZE];

	float unitsWithMetric[BASIC_SIZE + SMALL_UNIT_SIZE];
	float buildingsWithMetric[BASIC_SIZE + SMALL_BUILDING_SIZE];

	std::span<float> resourceIdInputSpan = std::span(resourceIdInput);
	std::span<float> unitsInputSpan = std::span(unitsInput);
	std::span<float> buildingsInputSpan = std::span(buildingsInput);

	std::span<float> unitsWithMetricUnitSpan = std::span(unitsWithMetric);
	std::span<float> basicWithMetricUnitSpan = std::span(buildingsWithMetric);
};
