#pragma once
#include <functional>

#include "AiConsts.h"
#include "player/Player.h"
#include "env/Environment.h"
#include "Game.h"
#include "env/influence/CenterType.h"


const inline struct MetricDefinitions {

	const std::vector<float>& getAiPlayerMetricNorm(Player* one, Player* two, std::span<AiPlayerMetric> metric) const {
		output.clear();
		for (auto const& v : metric) {
			output.push_back(v.fn(one, two) * v.weight);
		}
		return output;
	}

	const std::vector<float>& getUnitNormSmall(db_unit* unit, db_unit_level* level) const {
		outputSmall.clear();
		for (auto const& v : unitSmallInputSpan) {
			outputSmall.push_back(v.fn(unit, level) * v.weight);
		}
		return outputSmall;
	}

	const std::vector<float>& getUnitNormForSum(db_unit* unit, db_unit_level* level) const {
		outputSum.clear();
		for (auto const& v : unitInputSpan) {
			outputSum.push_back(v.fn(unit, level) * v.weightForSum);
		}
		return outputSum;
	}

	const std::vector<float>& getUnitNorm(db_unit* unit, db_unit_level* level) const {
		output.clear();
		for (auto const& v : unitInputSpan) {
			output.push_back(v.fn(unit, level) * v.weight);
		}
		return output;
	}

	const std::vector<float>& getBuildingNorm(db_building* building, db_building_level* level) const {
		output.clear();
		for (auto const& v : buildingInputSpan) {
			output.push_back(v.fn(building, level) * v.weight);
		}
		return output;
	}

	const std::vector<float>& getBuildingNormForSum(db_building* building, db_building_level* level) const {
		outputSum.clear();
		for (auto const& v : buildingInputSpan) {
			outputSum.push_back(v.fn(building, level) * v.weightForSum);
		}
		return outputSum;
	}

	const std::vector<float>& getResourceNorm(Resources& resources, Possession& possession) const {
		output.clear();
		for (auto const& v : resourceInputSpan) {
			output.push_back(v.fn(resources, possession) * v.weight);
		}
		return output;
	}

	const std::vector<float>& getBasicNorm(Player* one, Player* two) const {
		basic.clear();
		for (auto const& v : basicInputSpan) {
			output.push_back(v.fn(one, two) * v.weight);
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
		METRICS_OBJECTS.buildingSumCost,
		METRICS_OBJECTS.buildingMaxHp,
		METRICS_OBJECTS.buildingArmor,
		METRICS_OBJECTS.buildingSightRadius,

		METRICS_OBJECTS.buildingCollect,
		METRICS_OBJECTS.buildingAttack,//index5
		METRICS_OBJECTS.buildingAttackReload,
		METRICS_OBJECTS.buildingAttackRange,
		METRICS_OBJECTS.buildingResourceRange,

		METRICS_OBJECTS.buildingTypeCenter,
		METRICS_OBJECTS.buildingTypeHome,
		METRICS_OBJECTS.buildingTypeDefence,
		METRICS_OBJECTS.buildingTypeResourceFood,
		METRICS_OBJECTS.buildingTypeResourceWood,
		METRICS_OBJECTS.buildingTypeResourceStone,
		METRICS_OBJECTS.buildingTypeResourceGold,
		METRICS_OBJECTS.buildingTypeTechBlacksmith,
		METRICS_OBJECTS.buildingTypeTechUniversity,
		METRICS_OBJECTS.buildingTypeUnitBarracks,
		METRICS_OBJECTS.buildingTypeUnitRange,
		METRICS_OBJECTS.buildingTypeUnitCavalry,
	};

	static inline AiBuildingMetric aiBuildingOtherMetric[] = { //TODO moze cos wiecej?
		METRICS_OBJECTS.buildingTypeCenter,
		METRICS_OBJECTS.buildingTypeHome,
	};

	static inline AiBuildingMetric aiBuildingUnitsMetric[] = { //TODO moze cos wiecej?
		METRICS_OBJECTS.buildingTypeUnitBarracks,
		METRICS_OBJECTS.buildingTypeUnitRange,
		METRICS_OBJECTS.buildingTypeUnitCavalry,
	};

	static inline AiBuildingMetric aiBuildingTechMetric[] = { //TODO moze cos wiecej?
		METRICS_OBJECTS.buildingTypeTechBlacksmith,
		METRICS_OBJECTS.buildingTypeTechUniversity,
	};

	static inline AiBuildingMetric aiBuildingResMetric[] = {
		METRICS_OBJECTS.buildingCollect,
		METRICS_OBJECTS.buildingResourceRange,

		METRICS_OBJECTS.buildingTypeResourceFood,
		METRICS_OBJECTS.buildingTypeResourceWood,
		METRICS_OBJECTS.buildingTypeResourceStone,
		METRICS_OBJECTS.buildingTypeResourceGold,
	};

	static inline AiBuildingMetric aiBuildingDefMetric[] = {
		METRICS_OBJECTS.buildingSumCost,
		METRICS_OBJECTS.buildingMaxHp,
		METRICS_OBJECTS.buildingArmor,
		METRICS_OBJECTS.buildingSightRadius,

		METRICS_OBJECTS.buildingAttack,
		METRICS_OBJECTS.buildingAttackReload,
		METRICS_OBJECTS.buildingAttackRange,
	};

	static inline AiBuildingMetric aiSmallBuildingMetric[] = {
		METRICS_OBJECTS.buildingTypeCenter,
		METRICS_OBJECTS.buildingTypeHome,
		METRICS_OBJECTS.buildingTypeDefence,
		METRICS_OBJECTS.buildingTypeResourceFood,
		METRICS_OBJECTS.buildingTypeResourceWood,
		METRICS_OBJECTS.buildingTypeResourceStone,
		METRICS_OBJECTS.buildingTypeResourceGold,
		METRICS_OBJECTS.buildingTypeTechBlacksmith,
		METRICS_OBJECTS.buildingTypeTechUniversity,
		METRICS_OBJECTS.buildingTypeUnitBarracks,
		METRICS_OBJECTS.buildingTypeUnitRange,
		METRICS_OBJECTS.buildingTypeUnitCavalry,
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

	constexpr static std::span<AiBuildingMetric> aiBuildingDefInputSpan = std::span(aiBuildingDefMetric);
	constexpr static std::span<AiBuildingMetric> aiBuildingResInputSpan = std::span(aiBuildingResMetric);
	constexpr static std::span<AiBuildingMetric> aiBuildingTechInputSpan = std::span(aiBuildingTechMetric);
	constexpr static std::span<AiBuildingMetric> aiBuildingUnitsInputSpan = std::span(aiBuildingUnitsMetric);
	constexpr static std::span<AiBuildingMetric> aiBuildingOtherInputSpan = std::span(aiBuildingOtherMetric);

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
