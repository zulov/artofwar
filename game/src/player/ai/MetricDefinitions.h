#pragma once

#include <functional>
#include <span>

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
	template <typename T, typename L, typename MetricArray>
	void writeInput(std::span<float> output, T* one, L* two, const MetricArray& metrics) const {
		int i = 0;
		for (auto const& v : metrics) {
			output[i++] = v.fn(one, two) * v.weight;
		}
		assert(output.size() == i);
		assert(validateSpan(__LINE__, __FILE__, output));
	}

	void writeUnit(std::span<float> output, db_unit* unit, db_unit_level* level) const {
		writeInput(output, unit, level, aiUnitMetric);
	}

	void writeBuilding(std::span<float> output, db_building* building, db_building_level* level) const {
		writeInput(output, building, level, aiBuildingMetric);
	}

	void writeResource(std::span<float> output, Player* one, Player* two) const {
		writeInput(writeBasic(output, one, two), one->getResources(), one->getPossession(), aiResourceMetric);
	}

	void writeResourceWithOutBonus(std::span<float> output, Player* player, Player* enemy) const {
		writeInput(writeBasic(output, player, enemy), player->getResources(), player->getPossession(),
		           aiResourceWithoutBonusMetric);
	}

	const std::span<float> writeBasic(std::span<float> output, Player* one, Player* two) const {
		int i = 0;
		for (auto const& v : aiBasicMetric) {
			output[i++] = v.fn(one, two) * v.weight;
		}
		return std::span(output.begin() + i, output.size() - i);
	}

	//TODO te 3 sie troszke dubluj� ogran�� indeksami? ale z drugiej srony chce sie ich pozbyc
	void writeAttackOrDefence(std::span<float> output, Player* one, Player* two) const {
		writeInput(output, one, two, aiAttackOrDefence);
	}

	void writeWhereAttack(std::span<float> output, Player* one, Player* two) const {
		writeInput(writeBasic(output, one, two), one, two, aiWhereAttack);
	}

	void writeWhereDefend(std::span<float> output, Player* one, Player* two) const {
		writeInput(writeBasic(output, one, two), one, two, aiWhereDefend);
	}

	static float diffOfCenters(CenterType type1, Player* p1, CenterType type2, Player* p2, float defaultVal) {
		return Game::getEnvironment()->getDiffOfCenters(type1, p1->getId(), type2, p2->getId(), defaultVal);
	}

	const std::span<const unsigned char> getUnitTypesIdxs() const { return aiUnitsTypesIdxs; }
	const std::span<const unsigned char> getBuildingOtherIdxs() const { return aiBuildingOtherIdxs; }
	const std::span<const unsigned char> getBuildingDefenceIdxs() const { return aiBuildingDefIdxs; }
	const std::span<const unsigned char> getBuildingResourceIdxs() const { return aiBuildingResIdxs; }
	const std::span<const unsigned char> getBuildingTechIdxs() const { return aiBuildingTechIdxs; }
	const std::span<const unsigned char> getBuildingUnitsIdxs() const { return aiBuildingUnitsIdxs; }
	const std::span<const unsigned char> getBuildingTypesIdxs() const { return aiBuildingTypesIdxs; }


	static inline AiUnitMetric aiUnitMetric[] = { //db_unit* u, db_unit_level* l
		{[](auto u, auto l) -> float { return u->getSumCost(); }, 400},
		//TODO czy grupowe ma sens?
		{[](auto u, auto l) -> float { return l->maxHp; }, 300},
		{[](auto u, auto l) -> float { return l->armor; }},
		{[](auto u, auto l) -> float { return l->sightRadius; }, 20},

		{[](auto u, auto l) -> float { return l->collect; }},
		{[](auto u, auto l) -> float { return l->attack; }, 10}, //5
		{[](auto u, auto l) -> float { return l->attackReload; }, 200},
		{[](auto u, auto l) -> float { return l->attackRange; }, 20},

		{[](auto u, auto l) -> float { return u->typeInfantry; }}, //8
		{[](auto u, auto l) -> float { return u->typeRange; }},
		{[](auto u, auto l) -> float { return u->typeCalvary; }},
		{[](auto u, auto l) -> float { return u->typeWorker; }},
		{[](auto u, auto l) -> float { return u->typeSpecial; }},
		{[](auto u, auto l) -> float { return u->typeMelee; }},
		{[](auto u, auto l) -> float { return u->typeHeavy; }},
		{[](auto u, auto l) -> float { return u->typeLight; }}, //15

		{[](auto u, auto l) -> float { return l->bonusInfantry; }},
		{[](auto u, auto l) -> float { return l->bonusRange; }},
		{[](auto u, auto l) -> float { return l->bonusCalvary; }},
		{[](auto u, auto l) -> float { return l->bonusWorker; }},
		{[](auto u, auto l) -> float { return l->bonusSpecial; }},
		{[](auto u, auto l) -> float { return l->bonusMelee; }},
		{[](auto u, auto l) -> float { return l->bonusHeavy; }},
		{[](auto u, auto l) -> float { return l->bonusLight; }},
	};

	static inline AiBuildingMetric aiBuildingMetric[] = { //db_building* b, db_building_level* l
		{[](auto b, auto l) -> float { return b->getSumCost(); }, 400},
		{[](auto b, auto l) -> float { return l->maxHp; }, 500},
		{[](auto b, auto l) -> float { return l->armor; }, 1},
		{[](auto b, auto l) -> float { return l->sightRadius; }, 50},

		{[](auto b, auto l) -> float { return l->collect; }, 2},
		{[](auto b, auto l) -> float { return l->attack; }, 20}, //5
		{[](auto b, auto l) -> float { return l->attackReload; }, 200},
		{[](auto b, auto l) -> float { return l->attackRange; }, 20},
		{[](auto b, auto l) -> float { return l->resourceRange; }, 20},
		//TODO stad duzo wyrzuci�

		{[](auto b, auto l) -> float { return b->typeCenter; }}, //9
		{[](auto b, auto l) -> float { return b->typeHome; }},
		{[](auto b, auto l) -> float { return b->typeDefence; }},
		{[](auto b, auto l) -> float { return b->typeResourceFood; }}, //12
		{[](auto b, auto l) -> float { return b->typeResourceWood; }},
		{[](auto b, auto l) -> float { return b->typeResourceStone; }},
		{[](auto b, auto l) -> float { return b->typeResourceGold; }}, //15
		{[](auto b, auto l) -> float { return b->typeTechBlacksmith; }}, //16
		{[](auto b, auto l) -> float { return b->typeTechUniversity; }},
		{[](auto b, auto l) -> float { return b->typeUnitBarracks; }}, //18
		{[](auto b, auto l) -> float { return b->typeUnitRange; }},
		{[](auto b, auto l) -> float { return b->typeUnitCavalry; }},
	};

	//TODO moze to zwracac od razy przedzia�em jakos
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

	};

	static inline AiResourceMetric aiResourceWithoutBonusMetric[] = { //Resources* r, Possession* p
		{[](auto r, auto p) -> float { return p->getResWithOutBonus()[0]; }, 20},
		{[](auto r, auto p) -> float { return p->getResWithOutBonus()[1]; }, 20},
		{[](auto r, auto p) -> float { return p->getResWithOutBonus()[2]; }, 20},
		{[](auto r, auto p) -> float { return p->getResWithOutBonus()[3]; }, 20}
	};

	static inline AiPlayerMetric aiBasicMetric[] = { //Player* p1, Player* p2
		{[](auto p1, auto p2) -> float { return p1->getScore(); }, 1000},
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getUnitsNumber(); }, 200},
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getBuildingsNumber(); }, 50},

		{[](auto p1, auto p2) -> float { return p2->getScore(); }, 1000}
	};

	static inline AiPlayerMetric aiAttackOrDefence[] = { //Player* p1, Player* p2
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getAttackSum(); }, 1000},
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getDefenceAttackSum(); }, 100},
		{[](auto p1, auto p2) -> float { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p1, 0.f); }},
		{[](auto p1, auto p2) -> float { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p2, 1.f); }},
		{[](auto p1, auto p2) -> float { return diffOfCenters(CenterType::ARMY, p2, CenterType::BUILDING, p1, 1.f); }},
		{[](auto p1, auto p2) -> float { return diffOfCenters(CenterType::ARMY, p2, CenterType::BUILDING, p2, 0.f); }},
	};

	static inline AiPlayerMetric aiWhereAttack[] = { //Player* p1, Player* p2
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getAttackSum(); }, 1000},
		{[](auto p1, auto p2) -> float { return diffOfCenters(CenterType::ARMY, p1, CenterType::ECON, p2, 1.f); }},
		{[](auto p1, auto p2) -> float { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p2, 1.f); }},
		{[](auto p1, auto p2) -> float { return diffOfCenters(CenterType::ARMY, p1, CenterType::ARMY, p2, 1.f); }},
	};

	static inline AiPlayerMetric aiWhereDefend[] = { //Player* p1, Player* p2
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getAttackSum(); }, 1000},
		{[](auto p1, auto p2) -> float { return p1->getPossession()->getDefenceAttackSum(); }, 100},
		{[](auto p1, auto p2) -> float { return diffOfCenters(CenterType::ARMY, p1, CenterType::ECON, p1, 0.f); }},
		{[](auto p1, auto p2) -> float { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p1, 0.f); }},
		{[](auto p1, auto p2) -> float { return diffOfCenters(CenterType::ARMY, p1, CenterType::ARMY, p2, 1.f); }}
		//TODO musi by� do przeciwnika bo inaczej zawsze do siebie
	};

	//to nie musi byc array tylko tabelka
	//TODO improve nie indeksy ale enumy?, albo zawsze nowa tablica, beda powtorzenia
	constexpr static unsigned char aiUnitsTypesIdxs[] = {8, 9, 10, 11, 12, 12, 13, 14, 15};

	constexpr static unsigned char aiBuildingOtherIdxs[] = {9, 10}; //TODO moze cos wiecej?
	constexpr static unsigned char aiBuildingUnitsIdxs[] = {18, 19, 20};
	//TODO moze cos wiecej?
	constexpr static unsigned char aiBuildingTechIdxs[] = {16, 17}; //TODO moze cos wiecej?
	constexpr static unsigned char aiBuildingResIdxs[] = {4, 8, 12, 13, 14, 15};
	constexpr static unsigned char aiBuildingDefIdxs[] = {0, 1, 2, 3, 5, 6, 7};
	constexpr static unsigned char aiBuildingTypesIdxs[] = {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
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
constexpr char BUILDING_RES_BONUS_SIZE = std::size(METRIC_DEFINITIONS.aiResourceWithoutBonusMetric);

constexpr char BUILDING_TYPES_SIZE = std::size(METRIC_DEFINITIONS.aiBuildingTypesIdxs);

constexpr char RESOURCE_AI_SIZE = std::size(METRIC_DEFINITIONS.aiResourceMetric);

constexpr char ATTACK_OR_DEFENCE_SIZE = std::size(METRIC_DEFINITIONS.aiAttackOrDefence);
constexpr char WHERE_ATTACK_SIZE = std::size(METRIC_DEFINITIONS.aiWhereAttack);
constexpr char WHERE_DEFEND_SIZE = std::size(METRIC_DEFINITIONS.aiWhereDefend);
