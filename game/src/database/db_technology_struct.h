#pragma once

#include <algorithm>
#include <cassert>
#include <charconv>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <magic_enum.hpp>

#include "db_basic_struct.h"
#include "db_columns.h"
#include "db_struct.h"
#include "db_utils.h"
#include "utils/StringUtils.h"

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
inline std::optional<T> parseTechnologyEnum(const char* value) {
	const std::string_view name = value ? value : "";
	if constexpr (std::is_same_v<T, TechnologyTargetKind>) {
		if (name.empty()) return TechnologyTargetKind::NONE;
	}
	return magic_enum::enum_cast<T>(name, magic_enum::case_insensitive);
}

inline std::vector<unsigned short> parseTechnologyIds(const char* value, const char* fieldName) {
	if (!value || *value == '\0') return {};
	const std::string input(value);
	if (input == "none") return {};

	const auto reportEmpty = [&]() {
		std::cerr << "[Database Error] Empty " << fieldName << " ID\n";
		std::exit(EXIT_FAILURE);
	};
	const auto reportInvalid = [](const char* name, const std::string& token) {
		std::cerr << "[Database Error] Invalid " << name << " ID: " << token << "\n";
		std::exit(EXIT_FAILURE);
	};
	const auto reportOutOfRange = [](const char* name, const std::string& token) {
		std::cerr << "[Database Error] " << name << " ID is out of range: " << token << "\n";
		std::exit(EXIT_FAILURE);
	};

	const auto tokens = split(input, ',');
	std::vector<unsigned short> result;
	result.reserve(tokens.size());
	for (const auto& token : tokens) {
		if (token.empty()) {
			reportEmpty();
		}

		unsigned short parsed = 0;
		const auto [end, error] = std::from_chars(token.data(), token.data() + token.size(), parsed);
		if (error == std::errc::result_out_of_range) reportOutOfRange(fieldName, token);
		if (error != std::errc{} || end != token.data() + token.size()) reportInvalid(fieldName, token);
		result.push_back(parsed);
	}
	if (input.back() == ',') reportEmpty();
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
