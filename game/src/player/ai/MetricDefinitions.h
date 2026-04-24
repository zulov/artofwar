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

	using U = UnitMetricIdx;
	using B = BuildingMetricIdx;
	using R = ResourceMetricIdx;
	using RNB = ResourceNoBonusMetricIdx;
	using BM = BasicMetricIdx;
	using AD = AttackOrDefenceMetricIdx;
	using WA = WhereAttackMetricIdx;
	using WD = WhereDefendMetricIdx;

	static inline auto aiUnitMetric = buildMetricArray<U, AiUnitMetric>({
		{U::COST,            {[](auto u, auto l) -> float { return u->getSumCost(); }, 400}},
		{U::MAX_HP,          {[](auto u, auto l) -> float { return l->maxHp; }, 300}},
		{U::ARMOR,			{[](auto u, auto l) -> float { return l->armor; }}},
		{U::SIGHT_RADIUS,		{[](auto u, auto l) -> float { return l->sightRadius; }, 20}},
		{U::COLLECT,			{[](auto u, auto l) -> float { return l->collect; }}},
		{U::ATTACK,			{[](auto u, auto l) -> float { return l->attack; }, 10}},
		{U::ATTACK_RELOAD,   {[](auto u, auto l) -> float { return l->attackReload; }, 200}},
		{U::ATTACK_RANGE,    {[](auto u, auto l) -> float { return l->attackRange; }, 20}},
		{U::TYPE_INFANTRY,   {[](auto u, auto l) -> float { return u->typeInfantry; }}},
		{U::TYPE_RANGE,      {[](auto u, auto l) -> float { return u->typeRange; }}},
		{U::TYPE_CAVALRY,    {[](auto u, auto l) -> float { return u->typeCavalry; }}},
		{U::TYPE_WORKER,     {[](auto u, auto l) -> float { return u->typeWorker; }}},
		{U::TYPE_SPECIAL,    {[](auto u, auto l) -> float { return u->typeSpecial; }}},
		{U::TYPE_MELEE,      {[](auto u, auto l) -> float { return u->typeMelee; }}},
		{U::TYPE_HEAVY,      {[](auto u, auto l) -> float { return u->typeHeavy; }}},
		{U::TYPE_LIGHT,      {[](auto u, auto l) -> float { return u->typeLight; }}},
		{U::BONUS_INFANTRY,  {[](auto u, auto l) -> float { return l->bonusInfantry; }}},
		{U::BONUS_RANGE,     {[](auto u, auto l) -> float { return l->bonusRange; }}},
		{U::BONUS_CAVALRY,   {[](auto u, auto l) -> float { return l->bonusCavalry; }}},
		{U::BONUS_WORKER,    {[](auto u, auto l) -> float { return l->bonusWorker; }}},
		{U::BONUS_SPECIAL,   {[](auto u, auto l) -> float { return l->bonusSpecial; }}},
		{U::BONUS_MELEE,     {[](auto u, auto l) -> float { return l->bonusMelee; }}},
		{U::BONUS_HEAVY,     {[](auto u, auto l) -> float { return l->bonusHeavy; }}},
		{U::BONUS_LIGHT,     {[](auto u, auto l) -> float { return l->bonusLight; }}},
	});

	static inline auto aiBuildingMetric = buildMetricArray<B, AiBuildingMetric>({
		{B::COST,               {[](auto b, auto l) -> float { return b->getSumCost(); }, 400}},
		{B::MAX_HP,             {[](auto b, auto l) -> float { return l->maxHp; }, 500}},
		{B::ARMOR,              {[](auto b, auto l) -> float { return l->armor; }, 1}},
		{B::SIGHT_RADIUS,       {[](auto b, auto l) -> float { return l->sightRadius; }, 50}},
		{B::COLLECT,            {[](auto b, auto l) -> float { return l->collect; }, 2}},
		{B::ATTACK,             {[](auto b, auto l) -> float { return l->attack; }, 20}},
		{B::ATTACK_RELOAD,      {[](auto b, auto l) -> float { return l->attackReload; }, 200}},
		{B::ATTACK_RANGE,       {[](auto b, auto l) -> float { return l->attackRange; }, 20}},
		{B::RESOURCE_RANGE,		{[](auto b, auto l) -> float { return l->resourceRange; }, 20}},
		{B::TYPE_CENTER,        {[](auto b, auto l) -> float { return b->typeCenter; }}},
		{B::TYPE_HOME,          {[](auto b, auto l) -> float { return b->typeHome; }}},
		{B::TYPE_DEFENCE,       {[](auto b, auto l) -> float { return b->typeDefence; }}},
		{B::TYPE_RES_FOOD,      {[](auto b, auto l) -> float { return b->typeResourceFood; }}},
		{B::TYPE_RES_WOOD,      {[](auto b, auto l) -> float { return b->typeResourceWood; }}},
		{B::TYPE_RES_STONE,     {[](auto b, auto l) -> float { return b->typeResourceStone; }}},
		{B::TYPE_RES_GOLD,      {[](auto b, auto l) -> float { return b->typeResourceGold; }}},
		{B::TYPE_TECH_BLACKSMITH, {[](auto b, auto l) -> float { return b->typeTechBlacksmith; }}},
		{B::TYPE_TECH_UNIVERSITY, {[](auto b, auto l) -> float { return b->typeTechUniversity; }}},
		{B::TYPE_UNIT_BARRACKS, {[](auto b, auto l) -> float { return b->typeUnitBarracks; }}},
		{B::TYPE_UNIT_RANGE,    {[](auto b, auto l) -> float { return b->typeUnitRange; }}},
		{B::TYPE_UNIT_CAVALRY,  {[](auto b, auto l) -> float { return b->typeUnitCavalry; }}},
	});

	static inline auto aiResourceMetric = buildMetricArray<R, AiResourceMetric>({
		{R::GATHER_SPEED_FOOD,  {[](auto r, auto p) { return r->getGatherSpeeds()[cast(ResourceType::FOOD)]; }, 10}},
		{R::GATHER_SPEED_WOOD,  {[](auto r, auto p) { return r->getGatherSpeeds()[cast(ResourceType::WOOD)]; }, 10}},
		{R::GATHER_SPEED_STONE, {[](auto r, auto p) { return r->getGatherSpeeds()[cast(ResourceType::STONE)]; }, 10}},
		{R::GATHER_SPEED_GOLD,  {[](auto r, auto p) { return r->getGatherSpeeds()[cast(ResourceType::GOLD)]; }, 10}},
		{R::VALUE_FOOD,         {[](auto r, auto p) { return r->getValues()[cast(ResourceType::FOOD)]; }, 1000}},
		{R::VALUE_WOOD,         {[](auto r, auto p) { return r->getValues()[cast(ResourceType::WOOD)]; }, 1000}},
		{R::VALUE_STONE,        {[](auto r, auto p) { return r->getValues()[cast(ResourceType::STONE)]; }, 1000}},
		{R::VALUE_GOLD,         {[](auto r, auto p) { return r->getValues()[cast(ResourceType::GOLD)]; }, 1000}},
		{R::FREE_WORKERS,       {[](auto r, auto p) -> float { return p->getFreeWorkersNumber(); }, 100}},
		{R::TOTAL_WORKERS,      {[](auto r, auto p) -> float { return p->getWorkersNumber(); }, 100}},
	});

	static inline auto aiResourceWithoutBonusMetric = buildMetricArray<RNB, AiResourceMetric>({
		{RNB::VALUE_FOOD,  {[](auto r, auto p) { return p->getResWithOutBonus()[cast(ResourceType::FOOD)]; }, 20}},
		{RNB::VALUE_WOOD,  {[](auto r, auto p) { return p->getResWithOutBonus()[cast(ResourceType::WOOD)]; }, 20}},
		{RNB::VALUE_STONE, {[](auto r, auto p) { return p->getResWithOutBonus()[cast(ResourceType::STONE)]; }, 20}},
		{RNB::VALUE_GOLD,  {[](auto r, auto p) { return p->getResWithOutBonus()[cast(ResourceType::GOLD)]; }, 20}},
	});

	static inline auto aiBasicMetric = buildMetricArray<BM, AiPlayerMetric>({
		{BM::SCORE,           {[](auto p1, auto p2) -> float { return p1->getScore(); }, 1000}},
		{BM::UNITS_COUNT,     {[](auto p1, auto p2) -> float { return p1->getPossession()->getUnitsNumber(); }, 200}},
		{BM::BUILDINGS_COUNT, {[](auto p1, auto p2) -> float { return p1->getPossession()->getBuildingsNumber(); }, 50}},
		{BM::ENEMY_SCORE,     {[](auto p1, auto p2) -> float { return p2->getScore(); }, 1000}},
	});

	static inline auto aiAttackOrDefence = buildMetricArray<AD, AiPlayerMetric>({
		{AD::ATTACK_SUM,                  {[](auto p1, auto p2) { return p1->getPossession()->getAttackSum(); }, 1000}},
		{AD::DEFENCE_SUM,                 {[](auto p1, auto p2) { return p1->getPossession()->getDefenceAttackSum(); }, 100}},
		{AD::ARMY_TO_OWN_BUILDING,        {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p1, 0.f); }}},
		{AD::ARMY_TO_ENEMY_BUILDING,      {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p2, 1.f); }}},
		{AD::ENEMY_ARMY_TO_OWN_BUILDING,  {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p2, CenterType::BUILDING, p1, 1.f); }}},
		{AD::ENEMY_ARMY_TO_ENEMY_BUILDING,{[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p2, CenterType::BUILDING, p2, 0.f); }}},
	});

	static inline auto aiWhereAttack = buildMetricArray<WA, AiPlayerMetric>({
		{WA::ATTACK_SUM,            {[](auto p1, auto p2) { return p1->getPossession()->getAttackSum(); }, 1000}},
		{WA::ARMY_TO_ENEMY_ECON,    {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::ECON, p2, 1.f); }}},
		{WA::ARMY_TO_ENEMY_BUILDING,{[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p2, 1.f); }}},
		{WA::ARMY_TO_ENEMY_ARMY,    {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::ARMY, p2, 1.f); }}},
	});

	static inline auto aiWhereDefend = buildMetricArray<WD, AiPlayerMetric>({
		{WD::ATTACK_SUM,          {[](auto p1, auto p2) { return p1->getPossession()->getAttackSum(); }, 1000}},
		{WD::DEFENCE_SUM,         {[](auto p1, auto p2) { return p1->getPossession()->getDefenceAttackSum(); }, 100}},
		{WD::ARMY_TO_OWN_ECON,    {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::ECON, p1, 0.f); }}},
		{WD::ARMY_TO_OWN_BUILDING,{[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::BUILDING, p1, 0.f); }}},
		{WD::ARMY_TO_ENEMY_ARMY,  {[](auto p1, auto p2) { return diffOfCenters(CenterType::ARMY, p1, CenterType::ARMY, p2, 1.f); }}},
	});

	constexpr static unsigned char aiUnitsTypesIdxs[] = {
		idx(U::TYPE_INFANTRY), idx(U::TYPE_RANGE), idx(U::TYPE_CAVALRY), idx(U::TYPE_WORKER),
		idx(U::TYPE_SPECIAL),  idx(U::TYPE_MELEE), idx(U::TYPE_HEAVY),   idx(U::TYPE_LIGHT),
	};

	constexpr static unsigned char aiBuildingOtherIdxs[] = {
		idx(B::TYPE_CENTER), idx(B::TYPE_HOME),
	};
	constexpr static unsigned char aiBuildingUnitsIdxs[] = {
		idx(B::TYPE_UNIT_BARRACKS), idx(B::TYPE_UNIT_RANGE), idx(B::TYPE_UNIT_CAVALRY),
	};
	constexpr static unsigned char aiBuildingTechIdxs[] = {
		idx(B::TYPE_TECH_BLACKSMITH), idx(B::TYPE_TECH_UNIVERSITY),
	};
	constexpr static unsigned char aiBuildingResIdxs[] = {
		idx(B::COLLECT), idx(B::RESOURCE_RANGE),
		idx(B::TYPE_RES_FOOD), idx(B::TYPE_RES_WOOD), idx(B::TYPE_RES_STONE), idx(B::TYPE_RES_GOLD),
	};
	constexpr static unsigned char aiBuildingDefIdxs[] = {
		idx(B::COST), idx(B::MAX_HP), idx(B::ARMOR), idx(B::SIGHT_RADIUS),
		idx(B::ATTACK), idx(B::ATTACK_RELOAD), idx(B::ATTACK_RANGE),
	};
	constexpr static unsigned char aiBuildingTypesIdxs[] = {
		idx(B::TYPE_CENTER), idx(B::TYPE_HOME), idx(B::TYPE_DEFENCE),
		idx(B::TYPE_RES_FOOD), idx(B::TYPE_RES_WOOD), idx(B::TYPE_RES_STONE), idx(B::TYPE_RES_GOLD),
		idx(B::TYPE_TECH_BLACKSMITH), idx(B::TYPE_TECH_UNIVERSITY),
		idx(B::TYPE_UNIT_BARRACKS), idx(B::TYPE_UNIT_RANGE), idx(B::TYPE_UNIT_CAVALRY),
	};
	inline static std::array<float, METRIC_OUTPUT_MAX_SIZE> output{};
} METRIC_DEFINITIONS;

constexpr unsigned char BASIC_METRIC_SIZE = magic_enum::enum_count<BasicMetricIdx>();
constexpr unsigned char UNIT_METRIC_SIZE = magic_enum::enum_count<UnitMetricIdx>();
constexpr unsigned char BUILDING_METRIC_SIZE = magic_enum::enum_count<BuildingMetricIdx>();
