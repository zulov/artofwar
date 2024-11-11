#pragma once
#include <functional>

#include "player/Player.h"


struct AiMetric {
	const float weight;

	AiMetric(float weight) : weight(1 / weight) {}
};

struct AiUnitMetric : AiMetric {
	const std::function<float(db_unit* unit, db_unit_level* level)> fn;

	AiUnitMetric(const std::function<float(db_unit* unit, db_unit_level* level)>& fn, float weight = 1.f) : AiMetric(weight), fn(fn) {
	}
};

struct AiBuildingMetric : AiMetric {
	const std::function<float(db_building* building, db_building_level* level)> fn;

	AiBuildingMetric(const std::function<float(db_building* building, db_building_level* level)>& fn, float weight = 1.f) : AiMetric(weight), fn(fn) {
	}
};

struct AiResourceMetric : AiMetric {
	const std::function<float(Resources* resources, Possession* possession)> fn;

	AiResourceMetric(const std::function<float(Resources* resources, Possession* possession)>& fn,
	                 float weight) : AiMetric(weight), fn(fn) {
	}
};

struct AiPlayerMetric : AiMetric {
	const std::function<float(Player* one, Player* two)> fn;

	AiPlayerMetric(const std::function<float(Player* one, Player* two)>& fn,
	               float weight = 1.f) : AiMetric(weight), fn(fn) {
	}
};
