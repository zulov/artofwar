#pragma once

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
	using Fn = float (*)(db_unit* unit, db_unit_level* level);
	Fn fn = nullptr;

	AiUnitMetric() = default;
	AiUnitMetric(Fn fn, float weight = 1.f) : AiMetric(weight), fn(fn) {
	}
};

struct AiBuildingMetric : AiMetric {
	using Fn = float (*)(db_building* building, db_building_level* level);
	Fn fn = nullptr;

	AiBuildingMetric() = default;
	AiBuildingMetric(Fn fn, float weight = 1.f) : AiMetric(weight), fn(fn) {
	}
};

struct AiResourceMetric : AiMetric {
	using Fn = float (*)(Resources* resources, Possession* possession);
	Fn fn = nullptr;

	AiResourceMetric() = default;
	AiResourceMetric(Fn fn, float weight) : AiMetric(weight), fn(fn) {
	}
};

struct AiPlayerMetric : AiMetric {
	using Fn = float (*)(Player* one, Player* two);
	Fn fn = nullptr;

	AiPlayerMetric() = default;
	AiPlayerMetric(Fn fn, float weight = 1.f) : AiMetric(weight), fn(fn) {
	}
};
