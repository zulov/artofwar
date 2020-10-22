#pragma once
#include <magic_enum.hpp>
#include "objects/Metrics.h"

inline struct AiWeights {
	inline static float wUnitInput[magic_enum::enum_count<UnitMetric>()] = {
		500.f, //UnitMetric::DEFENCE
		100.f, //UnitMetric::DISTANCE_ATTACK
		100.f, //UnitMetric::CLOSE_ATTACK
		10.f, //UnitMetric::CHARGE_ATTACK
		100.f, //UnitMetric::BUILDING_ATTACK
		300.f //UnitMetric::COST_SUM
	};

	inline static float wBuildingInput[magic_enum::enum_count<BuildingMetric>()] = {
		2000.f, //BuildingMetric::DEFENCE
		100.f, //BuildingMetric::DISTANCE_ATTACK
		200.f, //BuildingMetric::PROD_DEFENCE
		200.f, //BuildingMetric::PROD_DISTANCE_ATTACK
		200.f, //BuildingMetric::PROD_CLOSE_ATTACK
		20.f, //BuildingMetric::PROD_CHARGE_ATTACK
		200.f, //BuildingMetric::PROD_BUILDING_ATTACK
		500.f //BuildingMetric::COST_SUM
	};

	constexpr static std::span<float> wUnitInputSpan = std::span(wUnitInput);
	constexpr static std::span<float> wBuildingInputSpan = std::span(wBuildingInput);

} AI_WEIGHTS;
