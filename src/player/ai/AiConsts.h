#pragma once
#include "AiMetric.h"
#include "player/Player.h"

constexpr char UNITS_SUM_X = 100;
constexpr char BUILDINGS_SUM_X = 10;

struct AiMetric {
	const float weight;
	const float weightForSum;

	AiMetric(float weight, float weightMultiplier) : weight(1 / weight), weightForSum(1 / (weight * weightMultiplier)) {
	}
};

struct AiUnitMetric : AiMetric {
	const std::function<float(db_unit* unit, db_unit_level* level)> fn;

	AiUnitMetric(const std::function<float(db_unit* unit, db_unit_level* level)>& fn, float weight,
	             float weightMultiplier = 1.f): fn(fn), AiMetric(weight, weightMultiplier) {
	}
};

struct AiBuildingMetric : AiMetric {
	const std::function<float(db_building* building, db_building_level* level)> fn;

	AiBuildingMetric(const std::function<float(db_building* building, db_building_level* level)>& fn, float weight,
	                 int weightMultiplier = 1.f): fn(fn), AiMetric(weight, weightMultiplier) {
	}
};

struct AiResourceMetric : AiMetric {
	const std::function<float(Resources& resources, Possession& possession)> fn;

	AiResourceMetric(const std::function<float(const Resources& resources, const Possession& possession)>& fn,
	                 float weight, float weightMultiplier = 1.f): fn(fn), AiMetric(weight, weightMultiplier) {
	}
};

struct AiPlayerMetric : AiMetric {
	const std::function<float(Player* one, Player* two)> fn;

	AiPlayerMetric(const std::function<float(Player* one, Player* two)>& fn,
	               float weight = 1.f, float weightMultiplier = 1.f): fn(fn), AiMetric(weight, weightMultiplier) {
	}
};

const inline struct MetricsObjects {

	static inline AiBuildingMetric buildingSumCost = {[](db_building* b, db_building_level* l) -> float { return b->getSumCost(); }, 400, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingMaxHp =	{[](db_building* b, db_building_level* l) -> float { return l->maxHp; }, 500, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingArmor =	{[](db_building* b, db_building_level* l) -> float { return l->armor; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingSightRadius=	{[](db_building* b, db_building_level* l) -> float { return l->sightRadius; }, 50, BUILDINGS_SUM_X};

	static inline AiBuildingMetric buildingCollect =	{[](db_building* b, db_building_level* l) -> float { return l->collect; }, 2, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingAttack =	{[](db_building* b, db_building_level* l) -> float { return l->attack; }, 20, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingAttackReload =	{[](db_building* b, db_building_level* l) -> float { return l->attackReload; }, 200, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingAttackRange =	{[](db_building* b, db_building_level* l) -> float { return l->attackRange; }, 20, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingResourceRange =	{[](db_building* b, db_building_level* l) -> float { return l->resourceRange; }, 20, BUILDINGS_SUM_X};

	static inline AiBuildingMetric buildingTypeCenter = {
		[](db_building* b, db_building_level* l) -> float { return b->typeCenter; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeHome = {
		[](db_building* b, db_building_level* l) -> float { return b->typeHome; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeDefence = {
		[](db_building* b, db_building_level* l) -> float { return b->typeDefence; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeResourceFood = {
		[](db_building* b, db_building_level* l) -> float { return b->typeResourceFood; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeResourceWood = {
		[](db_building* b, db_building_level* l) -> float { return b->typeResourceWood; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeResourceStone = {
		[](db_building* b, db_building_level* l) -> float { return b->typeResourceStone; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeResourceGold = {
		[](db_building* b, db_building_level* l) -> float { return b->typeResourceGold; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeTechBlacksmith = {
		[](db_building* b, db_building_level* l) -> float { return b->typeTechBlacksmith; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeTechUniversity = {
		[](db_building* b, db_building_level* l) -> float { return b->typeTechUniversity; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeUnitBarracks = {
		[](db_building* b, db_building_level* l) -> float { return b->typeUnitBarracks; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeUnitRange = {
		[](db_building* b, db_building_level* l) -> float { return b->typeUnitRange; }, 1, BUILDINGS_SUM_X};
	static inline AiBuildingMetric buildingTypeUnitCavalry = {
		[](db_building* b, db_building_level* l) -> float { return b->typeUnitCavalry; }, 1, BUILDINGS_SUM_X};
} METRICS_OBJECTS;
