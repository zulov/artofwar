#pragma once

#include <functional>
#include <span>
#include <vector>

#include "AiMetric.h"
#include "env/Environment.h"
#include "env/influence/CenterType.h"
#include "player/Player.h"
#include "player/Resources.h"
#include "player/Possession.h"

class Possession;
struct db_building_level;
struct db_building;
struct AiBuildingMetric;
struct db_unit_level;
struct db_unit;
struct AiPlayerMetric;
class Player;

constexpr inline struct MetricDefinitions {
	const std::vector<float>& getAiPlayerMetricNorm(Player* one, Player* two, std::span<AiPlayerMetric> metric) const {
		output.clear();
		for (auto const& v : metric) {
			output.push_back(v.fn(one, two) * v.weight);
		}
		return output;
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

	const std::vector<float> getBuildingNorm(std::span<AiBuildingMetric> span,
	                                         const std::function<float(const AiBuildingMetric&)>& getWeight,
	                                         db_building* building, db_building_level* level) const {
		std::vector<float> result;
		result.reserve(span.size());
		for (auto const& v : span) {
			result.push_back(v.fn(building, level) * getWeight(v));
		}
		return result;
	}

	const std::vector<float> getBuildingNorm(db_building* building, db_building_level* level) const {
		return getBuildingNorm(buildingInputSpan, [](const AiBuildingMetric& m) { return m.weight; }, building, level);
	}

	const std::vector<float> getBuildingNormForSum(db_building* building, db_building_level* level) const {
		return getBuildingNorm(buildingInputSpan, [](const AiBuildingMetric& m) { return m.weightForSum; }, building,
		                       level);
	}

	// const std::vector<float> getResourceNorm(Resources& resources, Possession& possession) const {
	// 	output.clear();
	// 	for (auto const& v : resourceAllInputSpan) {
	// 		output.push_back(v.fn(resources, possession) * v.weight);
	// 	}
	// 	return output;
	// }

	const std::vector<float> getResourceNorm(Resources* resources, Possession* possession,
	                                         std::span<unsigned char> idxs) const {
		output.clear();
		for (auto idx : idxs) {
			auto& v = resourceAllInputSpan[idx];
			output.push_back(v.fn(resources, possession) * v.weight);
		}
		return output;
	}

	const std::vector<float>& getBasicNorm(Player* one, Player* two) const {
		basic.clear();
		for (auto const& v : basicInputSpan) {
			basic.push_back(v.fn(one, two) * v.weight);
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

	const std::span<unsigned char> getUnitTypesIdxs() const { return aiUnitTypesIdxsSpan; }

	const std::span<unsigned char> getBuildingOtherIdxs() const { return aiBuildingOtherIdxsSpan; }
	const std::span<unsigned char> getBuildingDefenceIdxs() const { return aiBuildingDefIdxsSpan; }
	const std::span<unsigned char> getBuildingResourceIdxs() const { return aiBuildingResIdxsSpan; }
	const std::span<unsigned char> getBuildingTechIdxs() const { return aiBuildingTechIdxsSpan; }
	const std::span<unsigned char> getBuildingUnitsIdxs() const { return aiBuildingUnitsIdxsSpan; }
	const std::span<unsigned char> getBuildingTypesIdxs() const { return aiBuildingTypesIdxsSpan; }

	const std::span<unsigned char> getResWithoutBonusIdxs() const { return aiResWithoutBonusIdxsSpan; }


	static inline AiUnitMetric aiUnitMetric[] = { //db_unit* u, db_unit_level* l
		{[](auto u, auto l) -> float { return u->getSumCost(); }, 400, UNITS_SUM_X},
		//TODO czy grupowe ma sens?
		{[](auto u, auto l) -> float { return l->maxHp; }, 300, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->armor; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->sightRadius; }, 20, UNITS_SUM_X},
					
		{[](auto u, auto l) -> float { return l->collect; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->attack; }, 10, UNITS_SUM_X}, //index 5
		{[](auto u, auto l) -> float { return l->attackReload; }, 200, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->attackRange; }, 20, UNITS_SUM_X},
					
		{[](auto u, auto l) -> float { return u->typeInfantry; }, 1, UNITS_SUM_X}, //8
		{[](auto u, auto l) -> float { return u->typeRange; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return u->typeCalvary; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return u->typeWorker; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return u->typeSpecial; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return u->typeMelee; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return u->typeHeavy; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return u->typeLight; }, 1, UNITS_SUM_X}, //15
					
		{[](auto u, auto l) -> float { return l->bonusInfantry; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->bonusRange; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->bonusCalvary; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->bonusWorker; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->bonusSpecial; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->bonusMelee; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->bonusHeavy; }, 1, UNITS_SUM_X},
		{[](auto u, auto l) -> float { return l->bonusLight; }, 1, UNITS_SUM_X},
	};

	static inline AiBuildingMetric aiBuildingMetric[] = { //db_building* b, db_building_level* l
		{[](auto b, auto l) -> float { return b->getSumCost(); }, 400, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return l->maxHp; }, 500, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return l->armor; }, 1, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return l->sightRadius; }, 50, BUILDINGS_SUM_X},

		{[](auto b, auto l) -> float { return l->collect; }, 2, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return l->attack; }, 20, BUILDINGS_SUM_X}, //5
		{[](auto b, auto l) -> float { return l->attackReload; }, 200, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return l->attackRange; }, 20, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return l->resourceRange; }, 20, BUILDINGS_SUM_X},
		//TODO stad duzo wyrzuciæ

		{[](auto b, auto l) -> float { return b->typeCenter; }, 1, BUILDINGS_SUM_X}, //9
		{[](auto b, auto l) -> float { return b->typeHome; }, 1, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return b->typeDefence; }, 1, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return b->typeResourceFood; }, 1, BUILDINGS_SUM_X}, //12
		{[](auto b, auto l) -> float { return b->typeResourceWood; }, 1, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return b->typeResourceStone; }, 1, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return b->typeResourceGold; }, 1, BUILDINGS_SUM_X}, //15
		{[](auto b, auto l) -> float { return b->typeTechBlacksmith; }, 1, BUILDINGS_SUM_X}, //16
		{[](auto b, auto l) -> float { return b->typeTechUniversity; }, 1, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return b->typeUnitBarracks; }, 1, BUILDINGS_SUM_X}, //18
		{[](auto b, auto l) -> float { return b->typeUnitRange; }, 1, BUILDINGS_SUM_X},
		{[](auto b, auto l) -> float { return b->typeUnitCavalry; }, 1, BUILDINGS_SUM_X},
	};
	//TODO improve nie indeksy ale enumy?
	static inline unsigned char aiUnitsTypesIdxs[] = {8, 9, 10, 11, 12, 12, 13, 14, 15};

	static inline unsigned char aiBuildingOtherIdxs[] = {9, 10}; //TODO moze cos wiecej?
	static inline unsigned char aiBuildingUnitsIdxs[] = {18, 19, 20}; //TODO moze cos wiecej?
	static inline unsigned char aiBuildingTechIdxs[] = {16, 17}; //TODO moze cos wiecej?
	static inline unsigned char aiBuildingResIdxs[] = {4, 8, 12, 13, 14, 15};
	static inline unsigned char aiBuildingDefIdxs[] = {0, 1, 2, 3, 5, 6, 7};
	static inline unsigned char aiBuildingTypesIdxs[] = {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

	static inline unsigned char aiResInputIdxs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	static inline unsigned char aiResWithoutBonusIdxs[] = {10, 11, 12, 13};

	//TODO moze to zwracac od razy przedzia³em jakos
	static inline AiResourceMetric aiResourceMetric[] = { //Resources* r, Possession* p
		{[](auto r, auto p) -> float { return r->getGatherSpeeds()[0]; }, 10},
		{[](auto r, auto p) -> float { return r->getGatherSpeeds()[1]; }, 10},
		{[](auto r, auto p) -> float { return r->getGatherSpeeds()[2]; }, 10},
		{[](auto r, auto p) -> float { return r->getGatherSpeeds()[3]; }, 10},
					
		{[](auto r, auto p) -> float { return r->getValues()[0]; }, 1000},
		{[](auto r, auto p) -> float { return r->getValues()[1]; }, 1000},
		{[](auto r, auto p) -> float { return r->getValues()[2]; }, 1000},
		{[](auto r, auto p) -> float { return r->getValues()[3]; }, 1000},
					
		{[](auto r, auto p) -> float { return p->getFreeWorkersNumber(); }, 100},
		{[](auto r, auto p) -> float { return p->getWorkersNumber(); }, 100},
		{[](auto r, auto p) -> float { return p->getResWithOutBonus()[0]; }, 20}, //10
		{[](auto r, auto p) -> float { return p->getResWithOutBonus()[1]; }, 20},
		{[](auto r, auto p) -> float { return p->getResWithOutBonus()[2]; }, 20},
		{[](auto r, auto p) -> float { return p->getResWithOutBonus()[3]; }, 20},
	};

	static inline AiPlayerMetric aiBasicMetric[] = {//Player* p1, Player* p2
		{[](auto p1, auto p2) -> float { return p1->getScore(); }, 1000},
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getUnitsNumber(); }, 200},
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getBuildingsNumber(); }, 50},

		{[](auto p1, auto p2) -> float { return p2->getScore(); }, 1000}
	};

	static inline AiPlayerMetric aiAttackOrDefence[] = { //Player* p1, Player* p2
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getAttackSum(); }, 1000},
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getDefenceAttackSum(); }, 100},
		{
			[](auto p1, auto p2) -> float {
				return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::BUILDING,
				                                                p1->getId(), 0.f);
			}
		},
		{
			[](auto p1, auto p2) -> float {
				return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::BUILDING,
				                                                p2->getId(), 1.f);
			}
		},
		{
			[](auto p1, auto p2) -> float {
				return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p2->getId(), CenterType::BUILDING,
				                                                p1->getId(), 1.f);
			}
		},
		{
			[](auto p1, auto p2) -> float {
				return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p2->getId(), CenterType::BUILDING,
				                                                p2->getId(), 0.f);
			}
		},
	};

	static inline AiPlayerMetric aiWhereAttack[] = { //Player* p1, Player* p2
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getAttackSum(); }, 1000},
		{
			[](auto p1, auto p2) -> float {
				return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::ECON,
				                                                p2->getId(), 1.f);
			}
		},
		{
			[](auto p1, auto p2) -> float {
				return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::BUILDING,
				                                                p2->getId(), 1.f);
			}
		},
		{
			[](auto p1, auto p2) -> float {
				return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::ARMY,
				                                                p2->getId(), 1.f);
			}
		},
	};

	static inline AiPlayerMetric aiWhereDefend[] = { //Player* p1, Player* p2
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getAttackSum(); }, 1000},
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getDefenceAttackSum(); }, 100},
		{
			[](auto p1, auto p2) -> float {
				return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::ECON,
				                                                p1->getId(), 0.f);
			}
		},
		{
			[](auto p1, auto p2) -> float {
				return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::BUILDING,
				                                                p1->getId(), 0.f);
			}
		},
		{
			[](auto p1, auto p2) -> float {
				return Game::getEnvironment()->getDiffOfCenters(CenterType::ARMY, p1->getId(), CenterType::ARMY,
				                                                p2->getId(), 1.f);
			}
		},
		//TODO musi byæ do przeciwnika bo inaczej zawsze do siebie
	};
	//TODO pozbyc sie tych spanów
	constexpr static std::span<unsigned char> aiBuildingOtherIdxsSpan = std::span(aiBuildingOtherIdxs);
	constexpr static std::span<unsigned char> aiBuildingUnitsIdxsSpan = std::span(aiBuildingUnitsIdxs);
	constexpr static std::span<unsigned char> aiBuildingTechIdxsSpan = std::span(aiBuildingTechIdxs);
	constexpr static std::span<unsigned char> aiBuildingResIdxsSpan = std::span(aiBuildingResIdxs);
	constexpr static std::span<unsigned char> aiBuildingDefIdxsSpan = std::span(aiBuildingDefIdxs);
	constexpr static std::span<unsigned char> aiBuildingTypesIdxsSpan = std::span(aiBuildingTypesIdxs);

	constexpr static std::span<unsigned char> aiResWithoutBonusIdxsSpan = std::span(aiResWithoutBonusIdxs);
	constexpr static std::span<unsigned char> aiResInputIdxsSpan = std::span(aiResInputIdxs);

	constexpr static std::span<unsigned char> aiUnitTypesIdxsSpan = std::span(aiUnitsTypesIdxs);

	constexpr static std::span<AiUnitMetric> unitInputSpan = std::span(aiUnitMetric);

	constexpr static std::span<AiBuildingMetric> buildingInputSpan = std::span(aiBuildingMetric);

	constexpr static std::span<AiResourceMetric> resourceAllInputSpan = std::span(aiResourceMetric);
	constexpr static std::span<AiPlayerMetric> basicInputSpan = std::span(aiBasicMetric);

	constexpr static std::span<AiPlayerMetric> attackOrDefenceInputSpan = std::span(aiAttackOrDefence);
	constexpr static std::span<AiPlayerMetric> whereAttackInputSpan = std::span(aiWhereAttack);
	constexpr static std::span<AiPlayerMetric> whereDefendInputSpan = std::span(aiWhereDefend);

private:
	inline static std::vector<float> basic;
	inline static std::vector<float> output; //TODO mem perf mozna zastapic czyms lzejszym
	inline static std::vector<float> outputSum;
	//inline static std::vector<float> outputSmall;
} METRIC_DEFINITIONS;


constexpr char BASIC_SIZE = std::size(METRIC_DEFINITIONS.aiBasicMetric);
constexpr char UNIT_SIZE = std::size(METRIC_DEFINITIONS.aiUnitMetric);
constexpr char BUILDING_SIZE = std::size(METRIC_DEFINITIONS.aiBuildingMetric);
constexpr char UNIT_TYPES_SIZE = std::size(METRIC_DEFINITIONS.aiUnitsTypesIdxs);

constexpr char BUILDING_OTHER_SIZE = std::size(METRIC_DEFINITIONS.aiBuildingOtherIdxs);
constexpr char BUILDING_DEF_SIZE = std::size(METRIC_DEFINITIONS.aiBuildingDefIdxs);
constexpr char BUILDING_RES_SIZE = std::size(METRIC_DEFINITIONS.aiBuildingResIdxs);
constexpr char BUILDING_TECH_SIZE = std::size(METRIC_DEFINITIONS.aiBuildingTechIdxs);
constexpr char BUILDING_UNITS_SIZE = std::size(METRIC_DEFINITIONS.aiBuildingUnitsIdxs);
constexpr char BUILDING_RES_BONUS_SIZE = std::size(METRIC_DEFINITIONS.aiResWithoutBonusIdxs);

constexpr char BUILDING_TYPES_SIZE = std::size(METRIC_DEFINITIONS.aiBuildingTypesIdxs);

constexpr char RESOURCE_AI_SIZE = std::size(METRIC_DEFINITIONS.aiResInputIdxsSpan);

constexpr char ATTACK_OR_DEFENCE_SIZE = std::size(METRIC_DEFINITIONS.aiAttackOrDefence);
constexpr char WHERE_ATTACK_SIZE = std::size(METRIC_DEFINITIONS.aiWhereAttack);
constexpr char WHERE_DEFEND_SIZE = std::size(METRIC_DEFINITIONS.aiWhereDefend);
