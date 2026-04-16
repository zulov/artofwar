#pragma once

#include <functional>
#include <span>
#include <iterator>
#include <array>

#include <magic_enum.hpp>

#include "AiMetric.h"
#include "Game.h"
#include "env/Environment.h"
#include "env/influence/CenterType.h"
#include "player/Player.h"
#include "player/Resources.h"
#include "player/Possession.h"
#include "utils/SpanUtils.h"


constexpr unsigned char METRIC_OUTPUT_MAX_SIZE = 64;

enum class UnitMetricIdx : unsigned char {
	COST, MAX_HP, ARMOR, SIGHT_RADIUS,
	COLLECT, ATTACK, ATTACK_RELOAD, ATTACK_RANGE,
	TYPE_INFANTRY, TYPE_RANGE, TYPE_CAVALRY, TYPE_WORKER,
	TYPE_SPECIAL, TYPE_MELEE, TYPE_HEAVY, TYPE_LIGHT,
	BONUS_INFANTRY, BONUS_RANGE, BONUS_CAVALRY, BONUS_WORKER,
	BONUS_SPECIAL, BONUS_MELEE, BONUS_HEAVY, BONUS_LIGHT,
};

enum class BuildingMetricIdx : unsigned char {
	COST, MAX_HP, ARMOR, SIGHT_RADIUS,
	COLLECT, ATTACK, ATTACK_RELOAD, ATTACK_RANGE, RESOURCE_RANGE,
	TYPE_CENTER, TYPE_HOME, TYPE_DEFENCE,
	TYPE_RES_FOOD, TYPE_RES_WOOD, TYPE_RES_STONE, TYPE_RES_GOLD,
	TYPE_TECH_BLACKSMITH, TYPE_TECH_UNIVERSITY,
	TYPE_UNIT_BARRACKS, TYPE_UNIT_RANGE, TYPE_UNIT_CAVALRY,
};

enum class ResourceMetricIdx : unsigned char {
	GATHER_SPEED_FOOD, GATHER_SPEED_WOOD, GATHER_SPEED_STONE, GATHER_SPEED_GOLD,
	VALUE_FOOD, VALUE_WOOD, VALUE_STONE, VALUE_GOLD,
	FREE_WORKERS, TOTAL_WORKERS,
};

enum class ResourceNoBonusMetricIdx : unsigned char {
	VALUE_FOOD, VALUE_WOOD, VALUE_STONE, VALUE_GOLD,
};

enum class BasicMetricIdx : unsigned char {
	SCORE, UNITS_COUNT, BUILDINGS_COUNT, ENEMY_SCORE,
};

enum class AttackOrDefenceMetricIdx : unsigned char {
	ATTACK_SUM, DEFENCE_SUM,
	ARMY_TO_OWN_BUILDING, ARMY_TO_ENEMY_BUILDING,
	ENEMY_ARMY_TO_OWN_BUILDING, ENEMY_ARMY_TO_ENEMY_BUILDING,
};

enum class WhereAttackMetricIdx : unsigned char {
	ATTACK_SUM,
	ARMY_TO_ENEMY_ECON, ARMY_TO_ENEMY_BUILDING, ARMY_TO_ENEMY_ARMY,
};

enum class WhereDefendMetricIdx : unsigned char {
	ATTACK_SUM, DEFENCE_SUM,
	ARMY_TO_OWN_ECON, ARMY_TO_OWN_BUILDING, ARMY_TO_ENEMY_ARMY,
};

constexpr unsigned char idx(UnitMetricIdx e) { return static_cast<unsigned char>(e); }
constexpr unsigned char idx(BuildingMetricIdx e) { return static_cast<unsigned char>(e); }

template <typename Enum, typename Metric, size_t N>
auto buildMetricArray(std::pair<Enum, Metric>(&&entries)[N]) {
	constexpr auto SIZE = magic_enum::enum_count<Enum>();
	static_assert(N == SIZE, "Metric entry count must match enum size");
	std::array<Metric, SIZE> result{};
	bool filled[SIZE] = {};
	for (auto& [key, val] : entries) {
		auto i = static_cast<size_t>(key);
		assert(!filled[i] && "Duplicate metric enum key");
		filled[i] = true;
		result[i] = std::move(val);
	}
	for (size_t i = 0; i < SIZE; ++i) {
		assert(filled[i] && "Missing metric enum key");
	}
	return result;
}

inline struct MetricDefinitions {
	template <typename T, typename L, typename MetricArray>
	std::span<float> writeMetrics(T* one, L* two, const MetricArray& metrics) const {
		char size = std::size(metrics);
		assert(METRIC_OUTPUT_MAX_SIZE > size);
		int i = 0;
		for (auto const& v : metrics) {
			output[i++] = v.fn(one, two) * v.weight;
		}
		auto result = std::span(output.begin(), output.begin() + size);
		assert(i == size);
		assert(validateSpan(__LINE__, __FILE__, result));
		return result;
	}

	template <typename T, typename L, typename P, typename K, typename MetricArray>
	std::span<float> basicWithMetrics(T* one, L* two, P* three, K* four, const MetricArray& metrics) const {
		char size1 = std::size(metrics);
		char size = size1 + std::size(aiBasicMetric);
		assert(METRIC_OUTPUT_MAX_SIZE > size);
		int i = 0;
		for (auto const& v : aiBasicMetric) {
			output[i++] = v.fn(one, two) * v.weight;
		}
		for (auto const& v : metrics) {
			output[i++] = v.fn(three, four) * v.weight;
		}
		auto result = std::span(output.begin(), output.begin() + size);
		assert(i == size);
		assert(validateSpan(__LINE__, __FILE__, result));
		return result;
	}

	template <typename T, typename L>
	std::span<float> basicWithSpan(T* one, L* two, const std::span<const float> second) const {
		char size1 = std::size(second);
		char size = size1 + std::size(aiBasicMetric);
		assert(METRIC_OUTPUT_MAX_SIZE > size);
		int i = 0;
		for (auto const& v : aiBasicMetric) {
			output[i++] = v.fn(one, two) * v.weight;
		}
		for (auto const& v : second) {
			output[i++] = v;
		}
		auto result = std::span(output.begin(), output.begin() + size);
		assert(i == size);
		assert(validateSpan(__LINE__, __FILE__, result));
		return result;
	}

	template <typename T, typename L>
	std::span<float> basicWithSpanSelective(T* one, L* two, const std::span<const float> second, const std::span<const unsigned char> indxes) const {
		char size1 = std::size(indxes);
		char size = size1 + std::size(aiBasicMetric);
		assert(METRIC_OUTPUT_MAX_SIZE > size);
		int i = 0;
		for (auto const& v : aiBasicMetric) {
			output[i++] = v.fn(one, two) * v.weight;
		}
		for (auto j : indxes) {
			output[i++] = second[j];
		}
		auto result = std::span(output.begin(), output.begin() + size);
		assert(i == size);
		assert(validateSpan(__LINE__, __FILE__, result));
		return result;
	}

	const std::span<const float> writeUnit(db_unit* unit, db_unit_level* level) const {
		return writeMetrics(unit, level, aiUnitMetric);
	}

	const std::span<const float> writeBuilding(db_building* building, db_building_level* level) const {
		return writeMetrics(building, level, aiBuildingMetric);
	}

	const std::span<float> writeResource(Player* one, Player* two) const {
		return basicWithMetrics(one, two, one->getResources(), one->getPossession(), aiResourceMetric);
	}

	const std::span<float> writeResourceWithOutBonus(Player* player, Player* enemy) const {
		return basicWithMetrics(player, enemy, player->getResources(), player->getPossession(), aiResourceWithoutBonusMetric);
	}

	//TODO te 3 sie troszke dubluj¹ ogran¹æ indeksami? ale z drugiej srony chce sie ich pozbyc
	const std::span<float> writeAttackOrDefence(Player* one, Player* two) const {
		return writeMetrics(one, two, aiAttackOrDefence);
	}

	const std::span<float> writeWhereAttack(Player* one, Player* two) const {
		return basicWithMetrics(one, two, one, two, aiWhereAttack);
	}

	const std::span<float> writeWhereDefend(Player* one, Player* two) const {
		return basicWithMetrics(one, two, one, two, aiWhereDefend);
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

	static inline auto aiUnitMetric = buildMetricArray<UnitMetricIdx, AiUnitMetric>({
		//db_unit* u, db_unit_level* l
		{UnitMetricIdx::COST,            {[](auto u, auto l) -> float { return u->getSumCost(); }, 400}},
		{UnitMetricIdx::MAX_HP,          {[](auto u, auto l) -> float { return l->maxHp; }, 300}},
		{UnitMetricIdx::ARMOR,           {[](auto u, auto l) -> float { return l->armor; }}},
		{UnitMetricIdx::SIGHT_RADIUS,    {[](auto u, auto l) -> float { return l->sightRadius; }, 20}},

		{UnitMetricIdx::COLLECT,         {[](auto u, auto l) -> float { return l->collect; }}},
		{UnitMetricIdx::ATTACK,          {[](auto u, auto l) -> float { return l->attack; }, 10}},
		{UnitMetricIdx::ATTACK_RELOAD,   {[](auto u, auto l) -> float { return l->attackReload; }, 200}},
		{UnitMetricIdx::ATTACK_RANGE,    {[](auto u, auto l) -> float { return l->attackRange; }, 20}},

		{UnitMetricIdx::TYPE_INFANTRY,   {[](auto u, auto l) -> float { return u->typeInfantry; }}},
		{UnitMetricIdx::TYPE_RANGE,      {[](auto u, auto l) -> float { return u->typeRange; }}},
		{UnitMetricIdx::TYPE_CAVALRY,    {[](auto u, auto l) -> float { return u->typeCalvary; }}},
		{UnitMetricIdx::TYPE_WORKER,     {[](auto u, auto l) -> float { return u->typeWorker; }}},
		{UnitMetricIdx::TYPE_SPECIAL,    {[](auto u, auto l) -> float { return u->typeSpecial; }}},
		{UnitMetricIdx::TYPE_MELEE,      {[](auto u, auto l) -> float { return u->typeMelee; }}},
		{UnitMetricIdx::TYPE_HEAVY,      {[](auto u, auto l) -> float { return u->typeHeavy; }}},
		{UnitMetricIdx::TYPE_LIGHT,      {[](auto u, auto l) -> float { return u->typeLight; }}},

		{UnitMetricIdx::BONUS_INFANTRY,  {[](auto u, auto l) -> float { return l->bonusInfantry; }}},
		{UnitMetricIdx::BONUS_RANGE,     {[](auto u, auto l) -> float { return l->bonusRange; }}},
		{UnitMetricIdx::BONUS_CAVALRY,   {[](auto u, auto l) -> float { return l->bonusCalvary; }}},
		{UnitMetricIdx::BONUS_WORKER,    {[](auto u, auto l) -> float { return l->bonusWorker; }}},
		{UnitMetricIdx::BONUS_SPECIAL,   {[](auto u, auto l) -> float { return l->bonusSpecial; }}},
		{UnitMetricIdx::BONUS_MELEE,     {[](auto u, auto l) -> float { return l->bonusMelee; }}},
		{UnitMetricIdx::BONUS_HEAVY,     {[](auto u, auto l) -> float { return l->bonusHeavy; }}},
		{UnitMetricIdx::BONUS_LIGHT,     {[](auto u, auto l) -> float { return l->bonusLight; }}},
	});

	static inline auto aiBuildingMetric = buildMetricArray<BuildingMetricIdx, AiBuildingMetric>({
		//db_building* b, db_building_level* l
		{BuildingMetricIdx::COST,               {[](auto b, auto l) -> float { return b->getSumCost(); }, 400}},
		{BuildingMetricIdx::MAX_HP,             {[](auto b, auto l) -> float { return l->maxHp; }, 500}},
		{BuildingMetricIdx::ARMOR,              {[](auto b, auto l) -> float { return l->armor; }, 1}},
		{BuildingMetricIdx::SIGHT_RADIUS,       {[](auto b, auto l) -> float { return l->sightRadius; }, 50}},

		{BuildingMetricIdx::COLLECT,            {[](auto b, auto l) -> float { return l->collect; }, 2}},
		{BuildingMetricIdx::ATTACK,             {[](auto b, auto l) -> float { return l->attack; }, 20}},
		{BuildingMetricIdx::ATTACK_RELOAD,      {[](auto b, auto l) -> float { return l->attackReload; }, 200}},
		{BuildingMetricIdx::ATTACK_RANGE,       {[](auto b, auto l) -> float { return l->attackRange; }, 20}},
		{BuildingMetricIdx::RESOURCE_RANGE,     {[](auto b, auto l) -> float { return l->resourceRange; }, 20}},

		{BuildingMetricIdx::TYPE_CENTER,        {[](auto b, auto l) -> float { return b->typeCenter; }}},
		{BuildingMetricIdx::TYPE_HOME,          {[](auto b, auto l) -> float { return b->typeHome; }}},
		{BuildingMetricIdx::TYPE_DEFENCE,       {[](auto b, auto l) -> float { return b->typeDefence; }}},
		{BuildingMetricIdx::TYPE_RES_FOOD,      {[](auto b, auto l) -> float { return b->typeResourceFood; }}},
		{BuildingMetricIdx::TYPE_RES_WOOD,      {[](auto b, auto l) -> float { return b->typeResourceWood; }}},
		{BuildingMetricIdx::TYPE_RES_STONE,     {[](auto b, auto l) -> float { return b->typeResourceStone; }}},
		{BuildingMetricIdx::TYPE_RES_GOLD,      {[](auto b, auto l) -> float { return b->typeResourceGold; }}},
		{BuildingMetricIdx::TYPE_TECH_BLACKSMITH, {[](auto b, auto l) -> float { return b->typeTechBlacksmith; }}},
		{BuildingMetricIdx::TYPE_TECH_UNIVERSITY, {[](auto b, auto l) -> float { return b->typeTechUniversity; }}},
		{BuildingMetricIdx::TYPE_UNIT_BARRACKS, {[](auto b, auto l) -> float { return b->typeUnitBarracks; }}},
		{BuildingMetricIdx::TYPE_UNIT_RANGE,    {[](auto b, auto l) -> float { return b->typeUnitRange; }}},
		{BuildingMetricIdx::TYPE_UNIT_CAVALRY,  {[](auto b, auto l) -> float { return b->typeUnitCavalry; }}},
	});

	static inline auto aiResourceMetric = buildMetricArray<ResourceMetricIdx, AiResourceMetric>({
		//Resources* r, Possession* p
		{ResourceMetricIdx::GATHER_SPEED_FOOD,  {[](auto r, auto p) { return r->getGatherSpeeds()[0]; }, 10}},
		{ResourceMetricIdx::GATHER_SPEED_WOOD,  {[](auto r, auto p) { return r->getGatherSpeeds()[1]; }, 10}},
		{ResourceMetricIdx::GATHER_SPEED_STONE, {[](auto r, auto p) { return r->getGatherSpeeds()[2]; }, 10}},
		{ResourceMetricIdx::GATHER_SPEED_GOLD,  {[](auto r, auto p) { return r->getGatherSpeeds()[3]; }, 10}},

		{ResourceMetricIdx::VALUE_FOOD,         {[](auto r, auto p) { return r->getValues()[0]; }, 1000}},
		{ResourceMetricIdx::VALUE_WOOD,         {[](auto r, auto p) { return r->getValues()[1]; }, 1000}},
		{ResourceMetricIdx::VALUE_STONE,        {[](auto r, auto p) { return r->getValues()[2]; }, 1000}},
		{ResourceMetricIdx::VALUE_GOLD,         {[](auto r, auto p) { return r->getValues()[3]; }, 1000}},

		{ResourceMetricIdx::FREE_WORKERS,       {[](auto r, auto p) { return p->getFreeWorkersNumber(); }, 100}},
		{ResourceMetricIdx::TOTAL_WORKERS,      {[](auto r, auto p) { return p->getWorkersNumber(); }, 100}},
	});

	static inline auto aiResourceWithoutBonusMetric = buildMetricArray<ResourceNoBonusMetricIdx, AiResourceMetric>({
		//Resources* r, Possession* p
		{ResourceNoBonusMetricIdx::VALUE_FOOD,  {[](auto r, auto p) { return p->getResWithOutBonus()[0]; }, 20}},
		{ResourceNoBonusMetricIdx::VALUE_WOOD,  {[](auto r, auto p) { return p->getResWithOutBonus()[1]; }, 20}},
		{ResourceNoBonusMetricIdx::VALUE_STONE, {[](auto r, auto p) { return p->getResWithOutBonus()[2]; }, 20}},
		{ResourceNoBonusMetricIdx::VALUE_GOLD,  {[](auto r, auto p) { return p->getResWithOutBonus()[3]; }, 20}},
	});

	static inline auto aiBasicMetric = buildMetricArray<BasicMetricIdx, AiPlayerMetric>({
		//Player* p1, Player* p2
		{BasicMetricIdx::SCORE,            {[](auto p1, auto p2) { return p1->getScore(); }, 1000}},
		{BasicMetricIdx::UNITS_COUNT,      {[](auto p1, auto p2) { return p1->getPossession()->getUnitsNumber(); }, 200}},
		{BasicMetricIdx::BUILDINGS_COUNT,  {[](auto p1, auto p2) { return p1->getPossession()->getBuildingsNumber(); }, 50}},
		{BasicMetricIdx::ENEMY_SCORE,      {[](auto p1, auto p2) { return p2->getScore(); }, 1000}},
	});

	static inline auto aiAttackOrDefence = buildMetricArray<AttackOrDefenceMetricIdx, AiPlayerMetric>({
		//Player* p1, Player* p2
		{AttackOrDefenceMetricIdx::ATTACK_SUM,                  {[](auto p1, auto p2) { return p1->getPossession()->getAttackSum(); }, 1000}},
		{AttackOrDefenceMetricIdx::DEFENCE_SUM,                 {[](auto p1, auto p2) { return p1->getPossession()->getDefenceAttackSum(); }, 100}},
		{AttackOrDefenceMetricIdx::ARMY_TO_OWN_BUILDING,        {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p1, 0.f); }}},
		{AttackOrDefenceMetricIdx::ARMY_TO_ENEMY_BUILDING,      {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p2, 1.f); }}},
		{AttackOrDefenceMetricIdx::ENEMY_ARMY_TO_OWN_BUILDING,  {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p2, CenterType::BUILDING, p1, 1.f); }}},
		{AttackOrDefenceMetricIdx::ENEMY_ARMY_TO_ENEMY_BUILDING,{[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p2, CenterType::BUILDING, p2, 0.f); }}},
	});

	static inline auto aiWhereAttack = buildMetricArray<WhereAttackMetricIdx, AiPlayerMetric>({
		//Player* p1, Player* p2
		{WhereAttackMetricIdx::ATTACK_SUM,            {[](auto p1, auto p2) { return p1->getPossession()->getAttackSum(); }, 1000}},
		{WhereAttackMetricIdx::ARMY_TO_ENEMY_ECON,    {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::ECON, p2, 1.f); }}},
		{WhereAttackMetricIdx::ARMY_TO_ENEMY_BUILDING,{[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p2, 1.f); }}},
		{WhereAttackMetricIdx::ARMY_TO_ENEMY_ARMY,    {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::ARMY, p2, 1.f); }}},
	});

	static inline auto aiWhereDefend = buildMetricArray<WhereDefendMetricIdx, AiPlayerMetric>({
		//Player* p1, Player* p2
		{WhereDefendMetricIdx::ATTACK_SUM,            {[](auto p1, auto p2) { return p1->getPossession()->getAttackSum(); }, 1000}},
		{WhereDefendMetricIdx::DEFENCE_SUM,           {[](auto p1, auto p2) { return p1->getPossession()->getDefenceAttackSum(); }, 100}},
		{WhereDefendMetricIdx::ARMY_TO_OWN_ECON,      {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::ECON, p1, 0.f); }}},
		{WhereDefendMetricIdx::ARMY_TO_OWN_BUILDING,   {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p1, 0.f); }}},
		{WhereDefendMetricIdx::ARMY_TO_ENEMY_ARMY,     {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::ARMY, p2, 1.f); }}},
	});

	constexpr static unsigned char aiUnitsTypesIdxs[] = {
		idx(UnitMetricIdx::TYPE_INFANTRY), idx(UnitMetricIdx::TYPE_RANGE),
		idx(UnitMetricIdx::TYPE_CAVALRY),  idx(UnitMetricIdx::TYPE_WORKER),
		idx(UnitMetricIdx::TYPE_SPECIAL),  idx(UnitMetricIdx::TYPE_MELEE),
		idx(UnitMetricIdx::TYPE_HEAVY),    idx(UnitMetricIdx::TYPE_LIGHT),
	};

	constexpr static unsigned char aiBuildingOtherIdxs[] = {
		idx(BuildingMetricIdx::TYPE_CENTER), idx(BuildingMetricIdx::TYPE_HOME),
	};
	constexpr static unsigned char aiBuildingUnitsIdxs[] = {
		idx(BuildingMetricIdx::TYPE_UNIT_BARRACKS), idx(BuildingMetricIdx::TYPE_UNIT_RANGE),
		idx(BuildingMetricIdx::TYPE_UNIT_CAVALRY),
	};
	constexpr static unsigned char aiBuildingTechIdxs[] = {
		idx(BuildingMetricIdx::TYPE_TECH_BLACKSMITH), idx(BuildingMetricIdx::TYPE_TECH_UNIVERSITY),
	};
	constexpr static unsigned char aiBuildingResIdxs[] = {
		idx(BuildingMetricIdx::COLLECT), idx(BuildingMetricIdx::RESOURCE_RANGE),
		idx(BuildingMetricIdx::TYPE_RES_FOOD), idx(BuildingMetricIdx::TYPE_RES_WOOD),
		idx(BuildingMetricIdx::TYPE_RES_STONE), idx(BuildingMetricIdx::TYPE_RES_GOLD),
	};
	constexpr static unsigned char aiBuildingDefIdxs[] = {
		idx(BuildingMetricIdx::COST), idx(BuildingMetricIdx::MAX_HP),
		idx(BuildingMetricIdx::ARMOR), idx(BuildingMetricIdx::SIGHT_RADIUS),
		idx(BuildingMetricIdx::ATTACK), idx(BuildingMetricIdx::ATTACK_RELOAD),
		idx(BuildingMetricIdx::ATTACK_RANGE),
	};
	constexpr static unsigned char aiBuildingTypesIdxs[] = {
		idx(BuildingMetricIdx::TYPE_CENTER), idx(BuildingMetricIdx::TYPE_HOME),
		idx(BuildingMetricIdx::TYPE_DEFENCE),
		idx(BuildingMetricIdx::TYPE_RES_FOOD), idx(BuildingMetricIdx::TYPE_RES_WOOD),
		idx(BuildingMetricIdx::TYPE_RES_STONE), idx(BuildingMetricIdx::TYPE_RES_GOLD),
		idx(BuildingMetricIdx::TYPE_TECH_BLACKSMITH), idx(BuildingMetricIdx::TYPE_TECH_UNIVERSITY),
		idx(BuildingMetricIdx::TYPE_UNIT_BARRACKS), idx(BuildingMetricIdx::TYPE_UNIT_RANGE),
		idx(BuildingMetricIdx::TYPE_UNIT_CAVALRY),
	};
	inline static std::array<float, METRIC_OUTPUT_MAX_SIZE> output{};
} METRIC_DEFINITIONS;

constexpr unsigned char BASIC_METRIC_SIZE = magic_enum::enum_count<BasicMetricIdx>();
constexpr unsigned char UNIT_METRIC_SIZE = magic_enum::enum_count<UnitMetricIdx>();
constexpr unsigned char BUILDING_METRIC_SIZE = magic_enum::enum_count<BuildingMetricIdx>();
