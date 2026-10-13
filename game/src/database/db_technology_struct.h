#pragma once

#include <cassert>
#include <charconv>
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

enum class TechStat : unsigned char {
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
	TRAIN_TIME,
	FOOD_DECAY,
	GOLD_INTEREST
};

enum class TechOperation : unsigned char { ADD, PERCENT };

enum class TechSourceKind : unsigned char { UNIT, BUILDING, RESOURCE, PLAYER };

enum class TechSourceTag : unsigned char {
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

enum class TechTargetKind : unsigned char { NONE, UNIT, BUILDING, RESOURCE };

enum class TechResourceType : unsigned char { NONE, ANY, FOOD, WOOD, STONE, GOLD, INVALID };

template <typename T>
inline std::optional<T> parseTechEnum(const char* value) {
	const std::string_view name = value ? value : "";
	if constexpr (std::is_same_v<T, TechTargetKind> || std::is_same_v<T, TechResourceType>) {
		if (name.empty()) return T::NONE;
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

inline Urho3D::String technologyLevelKey(const Urho3D::String& code, unsigned char level) {
	return code + "_" + Urho3D::String(static_cast<unsigned>(level));
}

struct db_technology_effect : db_entity {
	const unsigned short technologyLevel;
	const TechStat stat;
	const TechOperation operation;
	const TechSourceKind sourceKind;
	const TechSourceTag sourceTag;
	const TechTargetKind targetKind;
	const TechSourceTag targetTag;
	const TechResourceType resourceType;
	const float value;

	using C = DbTechnologyEffectCol;
	db_technology_effect(sqlite3_stmt* stmt)
		: db_entity(asUShort(stmt, C::id)),
		  technologyLevel(asUShort(stmt, C::technology_level)),
		  stat(parseTechEnum<TechStat>(asText(stmt, C::stat)).value_or(TechStat::ATTACK)),
		  operation(parseTechEnum<TechOperation>(asText(stmt, C::operation)).value_or(TechOperation::ADD)),
		  sourceKind(parseTechEnum<TechSourceKind>(asText(stmt, C::source_kind)).value_or(TechSourceKind::PLAYER)),
		  sourceTag(parseTechEnum<TechSourceTag>(asText(stmt, C::source_tag)).value_or(TechSourceTag::ANY)),
		  targetKind(parseTechEnum<TechTargetKind>(asText(stmt, C::target_kind)).value_or(TechTargetKind::NONE)),
		  targetTag(parseTechEnum<TechSourceTag>(asText(stmt, C::target_tag)).value_or(TechSourceTag::ANY)),
		  resourceType(parseTechEnum<TechResourceType>(asText(stmt, C::resource_type)).value_or(TechResourceType::INVALID)),
		  value(asFloat(stmt, C::value)) {}
};

struct db_technology_level : db_with_name {
	const unsigned short technology;
	const unsigned char level;
	const Urho3D::String description;
	const Urho3D::String icon;
	const std::vector<unsigned short> unlockAgeIds;
	const db_with_cost cost;
	const short researchTime;
	std::vector<db_technology_effect*> effects;

	using C = DbTechnologyLevelCol;
	db_technology_level(sqlite3_stmt* stmt, const Urho3D::String& technologyCode)
		: db_with_name(asUShort(stmt, C::id), technologyLevelKey(technologyCode, asUByte(stmt, C::level))),
		  technology(asUShort(stmt, C::technology)), level(asUByte(stmt, C::level)),
		  description(technologyLevelKey(technologyCode, level) + "_description"),
		  icon(technologyLevelKey(technologyCode, level) + ".png"),
		  unlockAgeIds(parseTechnologyAgeIds(asText(stmt, C::unlock_age_ids))),
		  cost(asUShort(stmt, C::food), asUShort(stmt, C::wood), asUShort(stmt, C::stone), asUShort(stmt, C::gold)),
		  researchTime(asShort(stmt, C::research_time)) {}

	~db_technology_level() = default;
};

struct db_technology : db_with_name {
	const std::vector<unsigned short> researchBuildingIds;
	std::vector<db_technology_level*> levels;

	using C = DbTechnologyCol;
	db_technology(sqlite3_stmt* stmt)
		: db_with_name(asUShort(stmt, C::id), asText(stmt, C::code)),
		  researchBuildingIds(parseTechnologyBuildingIds(asText(stmt, C::research_building))) {}

	std::optional<db_technology_level*> getLevel(unsigned char level) const {
		return levels.size() > level ? std::optional(levels.at(level)) : std::nullopt;
	}
};
