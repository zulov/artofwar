#pragma once
#include <functional>
#include "player/Player.h"
#include "env/Environment.h"
#include "Game.h"
#include "env/influence/CenterType.h"


constexpr char UNITS_SUM_X = 100;
constexpr char BUILDINGS_SUM_X = 10;

struct AiMetric {
	const float weight;
	const float weightForSum;

	AiMetric(float weight, float weightMultiplier) : weight(weight), weightForSum(weight * weightMultiplier) {
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


const inline struct MetricDefinitions {

	MetricDefinitions() {
		const auto oSize = std::max({unitInputSpan.size(), buildingInputSpan.size(), resourceInputSpan.size(),
			basicInputSpan.size(), attackOrDefenceInputSpan.size(), whereAttackInputSpan.size(),
			whereDefendInputSpan.size()});
		const auto oSmallSize = std::max({unitSmallInputSpan.size(), buildingSmallInputSpan.size()});
		const auto oSizeSum = std::max({unitInputSpan.size(), buildingInputSpan.size()});
		basic.reserve(basicInputSpan.size());
		output.reserve(oSize);
		outputSmall.reserve(oSmallSize);
		outputSum.reserve(oSizeSum);
	}


	const std::vector<float>& getAiPlayerMetricNorm(Player* one, Player* two, std::span<AiPlayerMetric> metric) const {
		output.clear();
		for (auto const& v : metric) {
			output.push_back(v.fn(one, two) / v.weight);
		}
		return output;
	}

	const std::vector<float>& getUnitNormSmall(db_unit* unit, db_unit_level* level) const {
		outputSmall.clear();
		for (auto const& v : unitSmallInputSpan) {
			outputSmall.push_back(v.fn(unit, level) / v.weight);
		}
		return outputSmall;
	}

	const std::vector<float>& getUnitNormForSum(db_unit* unit, db_unit_level* level) const {
		outputSum.clear();
		for (auto const& v : unitInputSpan) {
			outputSum.push_back(v.fn(unit, level) / v.weightForSum);
		}
		return outputSum;
	}

	const std::vector<float>& getUnitNorm(db_unit* unit, db_unit_level* level) const {
		output.clear();
		for (auto const& v : unitInputSpan) {
			output.push_back(v.fn(unit, level) / v.weight);
		}
		return output;
	}

	const std::vector<float>& getBuildingNormSmall(db_building* building, db_building_level* level) const {
		outputSmall.clear();
		for (auto const& v : buildingSmallInputSpan) {
			outputSmall.push_back(v.fn(building, level) / v.weight);
		}
		return outputSmall;
	}

	const std::vector<float>& getBuildingNorm(db_building* building, db_building_level* level) const {
		output.clear();
		for (auto const& v : buildingInputSpan) {
			output.push_back(v.fn(building, level) / v.weight);
		}
		return output;
	}

	const std::vector<float>& getBuildingNormForSum(db_building* building, db_building_level* level) const {
		outputSum.clear();
		for (auto const& v : buildingInputSpan) {
			outputSum.push_back(v.fn(building, level) / v.weightForSum);
		}
		return outputSum;
	}

	const std::vector<float>& getResourceNorm(Resources& resources, Possession& possession) const {
		output.clear();
		for (auto const& v : resourceInputSpan) {
			output.push_back(v.fn(resources, possession) / v.weight);
		}
		return output;
	}

	const std::vector<float>& getBasicNorm(Player* one, Player* two) const {
		basic.clear();
		for (auto const& v : basicInputSpan) {
			output.push_back(v.fn(one, two) / v.weight);
		}
		return basic;
	}

	const std::vector<float>& getAttackOrDefenceNorm(Player* one, Player* two) const {
		return getAiPlayerMetricNorm(one, two, attackOrDefenceInputSpan);
	}

	const std::vector<float>& getWhereAttackNorm(Player* one, Player* two) const {
		return getAiPlayerMetricNorm(one, two, whereAttackInputSpan);
	}

	const std::vector<float>& getWhereDefendNorm(Player* one, Player* two) const {
		return getAiPlayerMetricNorm(one, two, whereDefendInputSpan);
	}


	static inline AiUnitMetric aiUnitMetric[] = {
		{[](db_unit* u, db_unit_level* l) -> float { return u->getSumCost(); }, 400, UNITS_SUM_X},
		//TODO czy grupowe ma sens?
		{[](db_unit* u, db_unit_level* l) -> float { return l->maxHp; }, 300, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->armor; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->sightRadius; }, 20, UNITS_SUM_X},

		{[](db_unit* u, db_unit_level* l) -> float { return l->collect; }, 1, UNITS_SUM_X},
		{[](db_unit* u, db_unit_level* l) -> float { return l->attack; }, 10, UNITS_SUM_X}, //index 5
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
		{[](db_building* b, db_building_level* l) -> float { return b->getSumCost(); }, 400, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->maxHp; }, 500, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->armor; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->sightRadius; }, 50, BUILDINGS_SUM_X},

		{[](db_building* b, db_building_level* l) -> float { return l->collect; }, 2, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->attack; }, 20, BUILDINGS_SUM_X}, //index 5
		{[](db_building* b, db_building_level* l) -> float { return l->attackReload; }, 200, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->attackRange; }, 20, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return l->resourceRange; }, 20, BUILDINGS_SUM_X},
		//TODO suma tego nie ma sensu flaga ze to nie ma sensu albo ujemna wartoœæ
		{[](db_building* b, db_building_level* l) -> float { return b->typeCenter; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeHome; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeDefence; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeResourceFood; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeResourceWood; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeResourceStone; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeResourceGold; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeTechBlacksmith; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeTechUniversity; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeUnitBarracks; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeUnitRange; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeUnitCavalry; }, 1, BUILDINGS_SUM_X},

	};


	static inline AiBuildingMetric aiSmallBuildingMetric[] = {
		{[](db_building* b, db_building_level* l) -> float { return b->typeCenter; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeHome; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeDefence; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeResourceFood; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeResourceWood; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeResourceStone; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeResourceGold; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeTechBlacksmith; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeTechUniversity; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeUnitBarracks; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeUnitRange; }, 1, BUILDINGS_SUM_X},
		{[](db_building* b, db_building_level* l) -> float { return b->typeUnitCavalry; }, 1, BUILDINGS_SUM_X},

	};

	//TODO moze to zwracac od razy przedzia³em jakos
	static inline AiResourceMetric aiResourceMetric[] = {
		{[](const Resources& r, const Possession& p) -> float { return r.getGatherSpeeds()[0]; }, 10},
		{[](const Resources& r, const Possession& p) -> float { return r.getGatherSpeeds()[1]; }, 10},
		{[](const Resources& r, const Possession& p) -> float { return r.getGatherSpeeds()[2]; }, 10},
		{[](const Resources& r, const Possession& p) -> float { return r.getGatherSpeeds()[3]; }, 10},

		{[](const Resources& r, const Possession& p) -> float { return r.getValues()[0]; }, 1000},
		{[](const Resources& r, const Possession& p) -> float { return r.getValues()[1]; }, 1000},
		{[](const Resources& r, const Possession& p) -> float { return r.getValues()[2]; }, 1000},
		{[](const Resources& r, const Possession& p) -> float { return r.getValues()[3]; }, 1000},

		{[](const Resources& r, const Possession& p) -> float { return p.getFreeWorkersNumber(); }, 100},
		{[](const Resources& r, const Possession& p) -> float { return p.getWorkersNumber(); }, 100},
	};

	static inline AiPlayerMetric aiBasicMetric[] = {
		{[](Player* one, Player* two) -> float { return one->getScore(); }, 1000},
		{[](Player* one, Player* two) -> float { return one->getPossession().getUnitsNumber(); }, 100},
		{[](Player* one, Player* two) -> float { return one->getPossession().getBuildingsNumber(); }, 100},

		{[](Player* one, Player* two) -> float { return two->getScore(); }, 1000},
		{[](Player* one, Player* two) -> float { return two->getPossession().getUnitsNumber(); }, 100},
		{[](Player* one, Player* two) -> float { return two->getPossession().getBuildingsNumber(); }, 100}
	};


	static inline AiPlayerMetric aiAttackOrDefence[] = {
		{[](Player* p1, Player* p2) -> float { return p1->getPossession().getAttackSum(); }, 1000},
		{[](Player* p1, Player* p2) -> float { return p1->getPossession().getDefenceAttackSum(); }, 100},
		{[](Player* p1, Player* p2) -> float { return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::BUILDING, p1->getId(), 0.f); }},
		{[](Player* p1, Player* p2) -> float { return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::BUILDING, p2->getId(), 1.f); }},
		{[](Player* p1, Player* p2) -> float { return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p2->getId(), CenterType::BUILDING, p1->getId(), 1.f); }},
		{[](Player* p1, Player* p2) -> float { return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p2->getId(), CenterType::BUILDING, p2->getId(), 0.f); }},
	};

	static inline AiPlayerMetric aiWhereAttack[] = {
		{[](Player* p1, Player* p2) -> float { return p1->getPossession().getAttackSum(); }, 1000},
		{[](Player* p1, Player* p2) -> float { return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::ECON, p2->getId(), 1.f); }},
		{[](Player* p1, Player* p2) -> float { return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::BUILDING, p2->getId(), 1.f); }},
		{[](Player* p1, Player* p2) -> float { return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::ARMY, p2->getId(), 1.f); }},
	};

	static inline AiPlayerMetric aiWhereDefend[] = {
		{[](Player* p1, Player* p2) -> float { return p1->getPossession().getAttackSum(); }, 1000},
		{[](Player* p1, Player* p2) -> float { return p1->getPossession().getDefenceAttackSum(); }, 100},
		{[](Player* p1, Player* p2) -> float { return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::ECON, p1->getId(), 0.f); }},
		{[](Player* p1, Player* p2) -> float { return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::BUILDING, p1->getId(), 0.f); }},
		{[](Player* p1, Player* p2) -> float { return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::ARMY, p2->getId(), 1.f); }},
		//TODO musi byæ do przeciwnika bo inaczej zawsze do siebie
	};

	constexpr static std::span<AiUnitMetric> unitSmallInputSpan = std::span(aiSmallUnitMetric);
	constexpr static std::span<AiUnitMetric> unitInputSpan = std::span(aiUnitMetric);

	constexpr static std::span<AiBuildingMetric> buildingSmallInputSpan = std::span(aiSmallBuildingMetric);
	constexpr static std::span<AiBuildingMetric> buildingInputSpan = std::span(aiBuildingMetric);

	constexpr static std::span<AiResourceMetric> resourceInputSpan = std::span(aiResourceMetric);
	constexpr static std::span<AiPlayerMetric> basicInputSpan = std::span(aiBasicMetric);

	constexpr static std::span<AiPlayerMetric> attackOrDefenceInputSpan = std::span(aiAttackOrDefence);
	constexpr static std::span<AiPlayerMetric> whereAttackInputSpan = std::span(aiWhereAttack);
	constexpr static std::span<AiPlayerMetric> whereDefendInputSpan = std::span(aiWhereDefend);

private:
	inline static std::vector<float> basic;
	inline static std::vector<float> output; //TODO mem perf mozna zastapic czyms lzejszym
	inline static std::vector<float> outputSum;
	inline static std::vector<float> outputSmall;

} METRIC_DEFINITIONS;
