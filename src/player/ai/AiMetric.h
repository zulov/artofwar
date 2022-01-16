#pragma once
#include <functional>
#include "player/Player.h"


#define UNITS_SUM_X 100
#define BUILDINGS_SUM_X 10

struct AiMetric {
	const float weight;
	const float weightForSum;

	AiMetric(float weight, float weightMultiplier) : weight(weight),   weightForSum(weight * weightMultiplier) {
	}
};

struct AiUnitMetric : AiMetric {
	const std::function<float(db_unit* unit, db_unit_level* level)> fn;

	AiUnitMetric(const std::function<float(db_unit* unit, db_unit_level* level)>& fn, float weight,
	             float weightMultiplier): fn(fn), AiMetric(weight, weightMultiplier) {
	}
};

struct AiBuildingMetric : AiMetric {
	const std::function<float(db_building* building, db_building_level* level)> fn;

	AiBuildingMetric(const std::function<float(db_building* building, db_building_level* level)>& fn, float weight,
	                 int weightMultiplier): fn(fn), AiMetric(weight, weightMultiplier) {
	}
};

struct AiResourceMetric : AiMetric {
	const std::function<float(Resources& resources, Possession& possession)> fn;

	AiResourceMetric(const std::function<float(const Resources& resources, const Possession& possession)>& fn,
	                 float weight, float weightMultiplier): fn(fn), AiMetric(weight, weightMultiplier) {
	}
};

struct AiBasicMetric : AiMetric {
	const std::function<float(Player* one, Player* two)> fn;

	AiBasicMetric(const std::function<float(Player* one, Player* two)>& fn,
	              float weight, float weightMultiplier): fn(fn), AiMetric(weight, weightMultiplier) {
	}
};

inline struct MetricDefinitions {

	const std::vector<float>& getUnitNormSmall(db_unit* unit, db_unit_level* level) {
		outputSmall.clear(); //TODO ensureSize once
		for (auto& v : unitSmallInputSpan) {
			outputSmall.push_back(v.fn(unit, level) / v.weight);
		}
		return outputSmall;
	}

	const std::vector<float>& getUnitNormForSum(db_unit* unit, db_unit_level* level) {
		outputSum.clear(); //TODO ensureSize once
		for (auto& v : unitInputSpan) {
			outputSum.push_back(v.fn(unit, level) / v.weightForSum);
		}
		return outputSum;
	}

	const std::vector<float>& getUnitNorm(db_unit* unit, db_unit_level* level) {
		output.clear(); //TODO ensureSize once
		for (auto& v : unitInputSpan) {
			output.push_back(v.fn(unit, level) / v.weight);
		}
		return output;
	}

	const std::vector<float>& getBuildingNormSmall(db_building* building, db_building_level* level) {
		outputSmall.clear(); //TODO ensureSize once
		for (auto& v : buildingSmallInputSpan) {
			outputSmall.push_back(v.fn(building, level) / v.weight);
		}
		return outputSmall;
	}

	const std::vector<float>& getBuildingNorm(db_building* building, db_building_level* level) {
		output.clear(); //TODO ensureSize once
		for (auto& v : buildingInputSpan) {
			output.push_back(v.fn(building, level) / v.weight);
		}
		return output;
	}

	const std::vector<float>& getBuildingNormForSum(db_building* building, db_building_level* level) {
		outputSum.clear(); //TODO ensureSize once
		for (auto& v : buildingInputSpan) {
			outputSum.push_back(v.fn(building, level) / v.weightForSum);
		}
		return outputSum;
	}

	const std::vector<float>& getResourceNorm(Resources& resources, Possession& possession) {
		output.clear(); //TODO ensureSize once
		for (auto& v : resourceInputSpan) {
			output.push_back(v.fn(resources, possession) / v.weight);
		}
		return output;
	}

	const std::vector<float>& getBasicNorm(Player* one, Player* two) {
		output.clear(); //TODO ensureSize once
		for (auto& v : basicInputSpan) {
			output.push_back(v.fn(one, two) / v.weight);
		}
		return output;
	}

	//TODO perf suma tego to tak naprawde zliczenie levelów i pomno¿enie bo kazda jednostka ma te same wartosci per level
	static inline AiUnitMetric aiUnitMetric[] = {
		{[](db_unit* unit, db_unit_level* level) -> float { return unit->getSumCost(); }, 400, UNITS_SUM_X},
		//TODO czy grupowe ma sens?
		{[](db_unit* u, db_unit_level* l) -> float { return l->maxHp; }, 300, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->armor; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->sightRadius; }, 20, UNITS_SUM_X},

		{[](db_unit* u, db_unit_level* l) -> float { return l->collect; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->attack; }, 10, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->attackReload; }, 200, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->attackRange; }, 20, UNITS_SUM_X},

		{[](db_unit* u, db_unit_level* l) -> float { return u->typeInfantry; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeRange; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeCalvary; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeWorker; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeSpecial; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeMelee; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeHeavy; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeLight; }, 1, UNITS_SUM_X},

		{[](db_unit* u, db_unit_level* l) -> float { return l->bonusInfantry; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->bonusRange; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->bonusCalvary; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->bonusWorker; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->bonusSpecial; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->bonusMelee; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->bonusHeavy; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->bonusLight; }, 1, UNITS_SUM_X},
	};

	static inline AiUnitMetric aiSmallUnitMetric[] = {
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeInfantry; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeRange; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeCalvary; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeWorker; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeSpecial; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeMelee; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeHeavy; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return u->typeLight; }, 1, UNITS_SUM_X}
	};

	static inline AiBuildingMetric aiBuildingMetric[] = {
		{[](db_building* b, db_building_level* l) -> float { return b->getSumCost(); }, 40, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->maxHp; }, 500, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->armor; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->sightRadius; }, 50, BUILDINGS_SUM_X},

		{[](db_building* b, db_building_level* l) -> float { return l->collect; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->attack; }, 20, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->attackReload; }, 200, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->attackRange; }, 20, BUILDINGS_SUM_X},
		//TODO suma tego nie ma sensu flaga ze to nie ma sensu albo ujemna wartoœæ
		{[](db_building* b, db_building_level* l) -> float { return b->typeDefence; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeResource; }, 4, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeTechnology; }, 2, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeCenter; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeUnit; }, 3, BUILDINGS_SUM_X}
	};

	
	static inline AiBuildingMetric aiSmallBuildingMetric[] = {
		{[](db_building* b, db_building_level* l) -> float { return b->typeDefence; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeResource; }, 4, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeTechnology; }, 2, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeCenter; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeUnit; }, 3, BUILDINGS_SUM_X}
	};

	//TODO moze to zwracac od razy przedzia³em jakos
	static inline AiResourceMetric aiResourceMetric[] = {
		{[](const Resources& r, const Possession& p) -> float { return r.getGatherSpeeds()[0]; }, 10, 1},
		{[](const Resources& r, const Possession& p) -> float { return r.getGatherSpeeds()[1]; }, 10, 1},
		{[](const Resources& r, const Possession& p) -> float { return r.getGatherSpeeds()[2]; }, 10, 1},
		{[](const Resources& r, const Possession& p) -> float { return r.getGatherSpeeds()[3]; }, 10, 1},

		{[](const Resources& r, const Possession& p) -> float { return r.getValues()[0]; }, 1000, 1},
		{[](const Resources& r, const Possession& p) -> float { return r.getValues()[1]; }, 1000, 1},
		{[](const Resources& r, const Possession& p) -> float { return r.getValues()[2]; }, 1000, 1},
		{[](const Resources& r, const Possession& p) -> float { return r.getValues()[3]; }, 1000, 1},

		{[](const Resources& r, const Possession& p) -> float { return p.getFreeWorkersNumber(); }, 100, 1},
		{[](const Resources& r, const Possession& p) -> float { return p.getWorkersNumber(); }, 100, 1},
	};

	static inline AiBasicMetric aiBasicMetric[] = {
		{[](Player* one, Player* two) -> float { return one->getScore(); }, 1000, 1},
		{[](Player* one, Player* two) -> float { return one->getPossession().getUnitsNumber(); }, 100, 1},
		{[](Player* one, Player* two) -> float { return one->getPossession().getBuildingsNumber(); }, 100, 1},

		{[](Player* one, Player* two) -> float { return two->getScore(); }, 1000, 1},
		{[](Player* one, Player* two) -> float { return two->getPossession().getUnitsNumber(); }, 100, 1},
		{[](Player* one, Player* two) -> float { return two->getPossession().getBuildingsNumber(); }, 100, 1}
	};

	constexpr static std::span<AiUnitMetric> unitSmallInputSpan = std::span(aiSmallUnitMetric);
	constexpr static std::span<AiUnitMetric> unitInputSpan = std::span(aiUnitMetric);

	constexpr static std::span<AiBuildingMetric> buildingSmallInputSpan = std::span(aiSmallBuildingMetric);
	constexpr static std::span<AiBuildingMetric> buildingInputSpan = std::span(aiBuildingMetric);

	constexpr static std::span<AiUnitMetric> smallUnitInputSpan = std::span(aiSmallUnitMetric);
	constexpr static std::span<AiBuildingMetric> smallBuildingInputSpan = std::span(aiSmallBuildingMetric);

	constexpr static std::span<AiResourceMetric> resourceInputSpan = std::span(aiResourceMetric);
	constexpr static std::span<AiBasicMetric> basicInputSpan = std::span(aiBasicMetric);

private: 
	inline static std::vector<float> output; //TODO mem perf mozna zastapic czyms lzejszym
	inline static std::vector<float> outputSum;
	inline static std::vector<float> outputSmall;

} METRIC_DEFINITIONS;
