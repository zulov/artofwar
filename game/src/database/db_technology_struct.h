#pragma once

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "db_basic_struct.h"
#include "db_columns.h"
#include "db_struct.h"
#include "db_utils.h"

enum class TechnologyStat : unsigned char {
	ATTACK,
	ARMOR,
	MAX_HP,
	SPEED,
	SIGHT_RANGE,
	ATTACK_RANGE,
	ATTACK_RELOAD,
	GATHER_RATE,
	RESOURCE_BONUS,
	RESOURCE_RANGE,
	FOOD_STORAGE,
	GOLD_STORAGE,
	STONE_REFINEMENT,
	GOLD_REFINEMENT,
	BUILD_TIME,
	TRAIN_TIME
};

enum class TechnologyOperation : unsigned char { ADD, PERCENT };

enum class TechnologySourceKind : unsigned char { UNIT, BUILDING, RESOURCE, PLAYER };

enum class TechnologySourceTag : unsigned char {
	ANY,
	ARMY,
	WORKER,
	INFANTRY,
	RANGED,
	CAVALRY,
	MELEE,
	HEAVY,
	LIGHT,
	SPECIAL,
	ALL_BUILDINGS,
	RESOURCE_BUILDING,
	DEFENSIVE_BUILDING,
	TECH_BUILDING
};

enum class TechnologyTargetKind : unsigned char { NONE, UNIT, BUILDING, RESOURCE };

inline std::string lowerTechnologyName(const char* value) {
	std::string result = value ? value : "";
	std::ranges::transform(result, result.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return result;
}

template <typename T>
std::optional<T> parseTechnologyEnum(const char* value);

template <>
inline std::optional<TechnologyStat> parseTechnologyEnum(const char* value) {
	const auto name = lowerTechnologyName(value);
	static constexpr std::pair<std::string_view, TechnologyStat> values[] = {
		{"attack", TechnologyStat::ATTACK}, {"armor", TechnologyStat::ARMOR}, {"max_hp", TechnologyStat::MAX_HP},
		{"speed", TechnologyStat::SPEED}, {"sight_range", TechnologyStat::SIGHT_RANGE},
		{"attack_range", TechnologyStat::ATTACK_RANGE}, {"attack_reload", TechnologyStat::ATTACK_RELOAD},
		{"gather_rate", TechnologyStat::GATHER_RATE}, {"resource_bonus", TechnologyStat::RESOURCE_BONUS},
		{"resource_range", TechnologyStat::RESOURCE_RANGE}, {"food_storage", TechnologyStat::FOOD_STORAGE},
		{"gold_storage", TechnologyStat::GOLD_STORAGE}, {"stone_refinement", TechnologyStat::STONE_REFINEMENT},
		{"gold_refinement", TechnologyStat::GOLD_REFINEMENT}, {"build_time", TechnologyStat::BUILD_TIME},
		{"train_time", TechnologyStat::TRAIN_TIME}};
	for (const auto& [candidate, parsed] : values) {
		if (name == candidate) return parsed;
	}
	return {};
}

template <>
inline std::optional<TechnologyOperation> parseTechnologyEnum(const char* value) {
	const auto name = lowerTechnologyName(value);
	if (name == "add") return TechnologyOperation::ADD;
	if (name == "percent") return TechnologyOperation::PERCENT;
	return {};
}

template <>
inline std::optional<TechnologySourceKind> parseTechnologyEnum(const char* value) {
	const auto name = lowerTechnologyName(value);
	if (name == "unit") return TechnologySourceKind::UNIT;
	if (name == "building") return TechnologySourceKind::BUILDING;
	if (name == "resource") return TechnologySourceKind::RESOURCE;
	if (name == "player") return TechnologySourceKind::PLAYER;
	return {};
}

template <>
inline std::optional<TechnologySourceTag> parseTechnologyEnum(const char* value) {
	const auto name = lowerTechnologyName(value);
	static constexpr std::pair<std::string_view, TechnologySourceTag> values[] = {
		{"any", TechnologySourceTag::ANY}, {"army", TechnologySourceTag::ARMY},
		{"worker", TechnologySourceTag::WORKER}, {"infantry", TechnologySourceTag::INFANTRY},
		{"ranged", TechnologySourceTag::RANGED}, {"cavalry", TechnologySourceTag::CAVALRY},
		{"melee", TechnologySourceTag::MELEE}, {"heavy", TechnologySourceTag::HEAVY},
		{"light", TechnologySourceTag::LIGHT}, {"special", TechnologySourceTag::SPECIAL},
		{"all_buildings", TechnologySourceTag::ALL_BUILDINGS},
		{"resource_building", TechnologySourceTag::RESOURCE_BUILDING},
		{"defensive_building", TechnologySourceTag::DEFENSIVE_BUILDING},
		{"tech_building", TechnologySourceTag::TECH_BUILDING}};
	for (const auto& [candidate, parsed] : values) {
		if (name == candidate) return parsed;
	}
	return {};
}

template <>
inline std::optional<TechnologyTargetKind> parseTechnologyEnum(const char* value) {
	const auto name = lowerTechnologyName(value);
	if (name.empty() || name == "none") return TechnologyTargetKind::NONE;
	if (name == "unit") return TechnologyTargetKind::UNIT;
	if (name == "building") return TechnologyTargetKind::BUILDING;
	if (name == "resource") return TechnologyTargetKind::RESOURCE;
	return {};
}

inline std::vector<unsigned short> parseTechnologyIds(const char* value, const char* fieldName) {
	std::vector<unsigned short> result;
	if (!value || *value == '\0') return {};
	std::string input(value);
	size_t begin = 0;
	while (begin <= input.size()) {
		const auto end = input.find(',', begin);
		const auto tokenEnd = end == std::string::npos ? input.size() : end;
		const auto token = input.substr(begin, tokenEnd - begin);
		if (token.empty()) {
			std::cerr << "[Database Error] Empty " << fieldName << " ID\n";
			std::exit(EXIT_FAILURE);
		}
		if (token == "none" && begin == 0 && end == std::string::npos) return {};
		unsigned long parsed = 0;
		for (const char character : token) {
			if (character < '0' || character > '9') {
				std::cerr << "[Database Error] Invalid " << fieldName << " ID: " << token << "\n";
				std::exit(EXIT_FAILURE);
			}
			const auto digit = static_cast<unsigned long>(character - '0');
			if (parsed > (std::numeric_limits<unsigned short>::max() - digit) / 10) {
				std::cerr << "[Database Error] " << fieldName << " ID is out of range: " << token << "\n";
				std::exit(EXIT_FAILURE);
			}
			parsed = parsed * 10 + digit;
		}
		result.push_back(static_cast<unsigned short>(parsed));
		if (end == std::string::npos) break;
		begin = end + 1;
	}
	return result;
}

inline std::vector<unsigned short> parseTechnologyAgeIds(const char* value) {
	return parseTechnologyIds(value, "technology age");
}

inline std::vector<unsigned short> parseTechnologyBuildingIds(const char* value) {
	return parseTechnologyIds(value, "technology research building");
}

inline std::string technologyLevelKey(const std::string& code, unsigned char level) {
	return code + "_" + std::to_string(level);
}

struct db_technology_effect : db_entity {
	const unsigned short technologyLevel;
	const unsigned short effectOrder;
	const TechnologyStat stat;
	const TechnologyOperation operation;
	const TechnologySourceKind sourceKind;
	const TechnologySourceTag sourceTag;
	const TechnologyTargetKind targetKind;
	const TechnologySourceTag targetTag;
	const std::string resourceType;
	const short targetId;
	const float value;

	using C = DbTechnologyEffectCol;
	db_technology_effect(sqlite3_stmt* stmt)
		: db_entity(asUShort(stmt, C::technology_level)),
		  technologyLevel(asUShort(stmt, C::technology_level)),
		  effectOrder(asUShort(stmt, C::effect_order)),
		  stat(parseTechnologyEnum<TechnologyStat>(asText(stmt, C::stat)).value_or(TechnologyStat::ATTACK)),
		  operation(parseTechnologyEnum<TechnologyOperation>(asText(stmt, C::operation)).value_or(TechnologyOperation::ADD)),
		  sourceKind(parseTechnologyEnum<TechnologySourceKind>(asText(stmt, C::source_kind)).value_or(TechnologySourceKind::PLAYER)),
		  sourceTag(parseTechnologyEnum<TechnologySourceTag>(asText(stmt, C::source_tag)).value_or(TechnologySourceTag::ANY)),
		  targetKind(parseTechnologyEnum<TechnologyTargetKind>(asText(stmt, C::target_kind)).value_or(TechnologyTargetKind::NONE)),
		  targetTag(parseTechnologyEnum<TechnologySourceTag>(asText(stmt, C::target_tag)).value_or(TechnologySourceTag::ANY)),
		  resourceType(asText(stmt, C::resource_type)), targetId(asShort(stmt, C::target_id)),
		  value(asFloat(stmt, C::value)) {}
};

struct db_technology_level : db_with_name {
	const unsigned short technology;
	const unsigned char level;
	const std::string description;
	const std::string icon;
	const std::vector<unsigned short> unlockAgeIds;
	const db_with_cost cost;
	const short researchTime;
	std::vector<db_technology_effect*> effects;

	using C = DbTechnologyLevelCol;
	db_technology_level(sqlite3_stmt* stmt, const std::string& technologyCode)
		: db_with_name(asUShort(stmt, C::id), technologyLevelKey(technologyCode, asUByte(stmt, C::level)).c_str()),
		  technology(asUShort(stmt, C::technology)), level(asUByte(stmt, C::level)),
		  description(technologyLevelKey(technologyCode, level) + "_description"),
		  icon(technologyLevelKey(technologyCode, level) + ".png"),
		  unlockAgeIds(parseTechnologyAgeIds(asText(stmt, C::unlock_age_ids))),
		  cost(asUShort(stmt, C::food), asUShort(stmt, C::wood), asUShort(stmt, C::stone), asUShort(stmt, C::gold)),
		  researchTime(asShort(stmt, C::research_time)) {}

	~db_technology_level() = default;
};

struct db_technology : db_with_name {
	const std::string code;
	const std::vector<unsigned short> researchBuildingIds;
	std::vector<db_technology_level*> levels;

	using C = DbTechnologyCol;
	db_technology(sqlite3_stmt* stmt)
		: db_with_name(asUShort(stmt, C::id), asText(stmt, C::code)), code(asText(stmt, C::code)),
		  researchBuildingIds(parseTechnologyBuildingIds(asText(stmt, C::research_building))) {}

	std::optional<db_technology_level*> getLevel(unsigned char level) const {
		return levels.size() > level ? std::optional(levels.at(level)) : std::nullopt;
	}
};
