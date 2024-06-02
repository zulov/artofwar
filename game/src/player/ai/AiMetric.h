#pragma once
#include <functional>

// #include "database/db_strcut.h"
#include "player/Player.h"

constexpr char UNITS_SUM_X = 100;
constexpr char BUILDINGS_SUM_X = 10;



struct AiMetric {
	const float weight;
	const float weightForSum;

	AiMetric(float weight, float weightMultiplier) : weight(1 / weight),
	                                                 weightForSum(1 / (weight * weightMultiplier)) {
	}
};

struct AiUnitMetric : AiMetric {
	const std::function<float(db_unit* unit, db_unit_level* level)> fn;

	AiUnitMetric(const std::function<float(db_unit* unit, db_unit_level* level)>& fn, float weight,
	             float weightMultiplier = 1.f) : AiMetric(weight, weightMultiplier), fn(fn) {
	}
};

struct AiBuildingMetric : AiMetric {
	const std::function<float(db_building* building, db_building_level* level)> fn;

	AiBuildingMetric(const std::function<float(db_building* building, db_building_level* level)>& fn, float weight,
	                 int weightMultiplier = 1.f) : AiMetric(weight, weightMultiplier), fn(fn) {
	}
};

struct AiResourceMetric : AiMetric {
	const std::function<float(Resources* resources, Possession* possession)> fn;

	AiResourceMetric(const std::function<float(Resources* resources, Possession* possession)>& fn,
	                 float weight, float weightMultiplier = 1.f) : AiMetric(weight, weightMultiplier), fn(fn) {
	}
};

struct AiPlayerMetric : AiMetric {
	const std::function<float(Player* one, Player* two)> fn;

	AiPlayerMetric(const std::function<float(Player* one, Player* two)>& fn,
	               float weight = 1.f, float weightMultiplier = 1.f) : AiMetric(weight, weightMultiplier), fn(fn) {
	}
};
