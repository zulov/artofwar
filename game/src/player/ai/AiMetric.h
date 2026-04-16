#pragma once
#include <functional>


class Player;
class Possession;
class Resources;
struct db_building_level;
struct db_building;
struct db_unit_level;
struct db_unit;

struct AiMetric {
	float weight;

	AiMetric() : weight(1.f) {}
	AiMetric(float weight) : weight(1 / weight) {}
};

struct AiUnitMetric : AiMetric {
	std::function<float(db_unit* unit, db_unit_level* level)> fn;

	AiUnitMetric() = default;
	AiUnitMetric(const std::function<float(db_unit* unit, db_unit_level* level)>& fn, float weight = 1.f) : AiMetric(weight), fn(fn) {
	}
};

struct AiBuildingMetric : AiMetric {
	std::function<float(db_building* building, db_building_level* level)> fn;

	AiBuildingMetric() = default;
	AiBuildingMetric(const std::function<float(db_building* building, db_building_level* level)>& fn, float weight = 1.f) : AiMetric(weight), fn(fn) {
	}
};

struct AiResourceMetric : AiMetric {
	std::function<float(Resources* resources, Possession* possession)> fn;

	AiResourceMetric() = default;
	AiResourceMetric(const std::function<float(Resources* resources, Possession* possession)>& fn,
	                 float weight) : AiMetric(weight), fn(fn) {
	}
};

struct AiPlayerMetric : AiMetric {
	std::function<float(Player* one, Player* two)> fn;

	AiPlayerMetric() = default;
	AiPlayerMetric(const std::function<float(Player* one, Player* two)>& fn,
	               float weight = 1.f) : AiMetric(weight), fn(fn) {
	}
};
