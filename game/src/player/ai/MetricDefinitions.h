#pragma once

#include <span>
#include <array>

#include <magic_enum.hpp>

#include "AiMetric.h"
#include "UnitBrain.h"
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
	ATTACK, ATTACK_RELOAD, ATTACK_RANGE,
	TYPE_INFANTRY, TYPE_RANGE, TYPE_CAVALRY,
	TYPE_SPECIAL, TYPE_MELEE, TYPE_HEAVY, TYPE_LIGHT,
	BONUS_INFANTRY, BONUS_RANGE, BONUS_CAVALRY, BONUS_WORKER,
	BONUS_SPECIAL, BONUS_MELEE, BONUS_HEAVY, BONUS_LIGHT,
	BONUS_BUILDING,
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

enum class ResourceEconomyMetricIdx : unsigned char {
	// Per-resource gather speeds & values (8)
	GATHER_SPEED_FOOD, GATHER_SPEED_WOOD, GATHER_SPEED_STONE, GATHER_SPEED_GOLD,
	VALUE_FOOD, VALUE_WOOD, VALUE_STONE, VALUE_GOLD,
	// Workers (2)
	FREE_WORKERS, TOTAL_WORKERS,
	// Storage signals (4)
	FOOD_STORAGE, FOOD_EXCESS, FOOD_STORAGE_RATIO, GOLD_STORAGE_RATIO,
	// Refine signals (4)
	STONE_REFINE_GAP, GOLD_REFINE_GAP, STONE_REFINE_UTIL, GOLD_REFINE_UTIL,
	// Building counts (7)
	BONUS_BUILDING_COUNT, SPAWNER_COUNT, CONVERT_BUILDING_COUNT,
	STORAGE_BUILDING_COUNT, REFINE_BUILDING_COUNT,
	// Per-resource building counts (4) - for bonus need balance
	RES_BUILDING_FOOD, RES_BUILDING_WOOD, RES_BUILDING_STONE, RES_BUILDING_GOLD,
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

template <typename MetricArray, typename T, typename L>
auto section(const MetricArray& metrics, T* one, L* two) {
	struct S {
		const MetricArray& metrics; T* one; L* two;
		int write(float* out) const {
			int i = 0;
			for (auto const& v : metrics) { out[i++] = v.fn(one, two) * v.weight; }
			return i;
		}
	};
	return S{metrics, one, two};
}

inline auto section(std::span<const float> span) {
	struct S {
		std::span<const float> span;
		int write(float* out) const {
			int i = 0;
			for (auto const& v : span) { out[i++] = v; }
			return i;
		}
	};
	return S{span};
}

inline auto section(std::span<const float> span, std::span<const unsigned char> indices) {
	struct S {
		std::span<const float> span; std::span<const unsigned char> indices;
		int write(float* out) const {
			int i = 0;
			for (auto j : indices) { out[i++] = span[j]; }
			return i;
		}
	};
	return S{span, indices};
}

inline struct MetricDefinitions {

	template <typename... Sections>
	std::span<float> compose(Sections&&... sections) const {
		int i = 0;
		((i += sections.write(output.data() + i)), ...);
		assert(METRIC_OUTPUT_MAX_SIZE > i);
		auto result = std::span(output.begin(), output.begin() + i);
		assert(validateSpan(__LINE__, __FILE__, result));
		return result;
	}

	std::span<const float> writeUnit(db_unit* unit, db_unit_level* level) const;
	std::span<const float> writeBuilding(db_building* building, db_building_level* level) const;


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
	using RE = ResourceEconomyMetricIdx;
	using BM = BasicMetricIdx;
	using AD = AttackOrDefenceMetricIdx;
	using WA = WhereAttackMetricIdx;
	using WD = WhereDefendMetricIdx;

	static constexpr size_t UNIT_METRIC_COUNT = magic_enum::enum_count<U>();
	static constexpr size_t BUILDING_METRIC_COUNT = magic_enum::enum_count<B>();
	static std::array<AiUnitMetric, UNIT_METRIC_COUNT> aiUnitMetric;
	static std::array<AiBuildingMetric, BUILDING_METRIC_COUNT> aiBuildingMetric;

	constexpr static unsigned char aiUnitsTypesIdxs[] = {
		idx(U::TYPE_INFANTRY), idx(U::TYPE_RANGE), idx(U::TYPE_CAVALRY),
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


static_assert(UNIT_PROFILE_SIZE == UNIT_METRIC_SIZE,
              "UnitBrain UNIT_PROFILE_SIZE must match the UnitMetricIdx count");
