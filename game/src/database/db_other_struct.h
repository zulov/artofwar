#pragma once
#include <charconv>
#include <cassert>
#include <string_view>
#include <system_error>
#include <vector>

#include <Urho3D/Math/Color.h>

#include "db_basic_struct.h"
#include "db_columns.h"
#include "db_utils.h"
#include "utils/DeleteUtils.h"
#include "db_world_age_struct.h"

#include <magic_enum.hpp>

enum class MissionGoalType : unsigned char {
	ELIMINATE_PLAYER,
	ELIMINATE_TEAM,
	ELIMINATE_ALL,
	ELIMINATE_UNITS,
	ELIMINATE_BUILDINGS,
	SURVIVE_WITH_BUILDING_UNTIL,
	SURVIVE_UNTIL,
	KILL_UNITS,
	SCORE_LEAD_AT_TIME,
	RESOURCE_STOCK,
	RESOURCE_GATHERED,
	BUILDING_COUNT,
	SCORE_REACH
};

struct db_mission_goal : db_entity {
	const MissionGoalType type;
	const Urho3D::String textKey;
	const short targetPlayer;
	const short targetId;
	const float targetValue;
	const unsigned timeTicks;

	using C = DbMissionGoalCol;
	explicit db_mission_goal(sqlite3_stmt* stmt)
		: db_entity(asUShort(stmt, C::id)),
		  type(magic_enum::enum_cast<MissionGoalType>(asText(stmt, C::type)).value_or(MissionGoalType::ELIMINATE_PLAYER)),
		  textKey(asText(stmt, C::text_key)),
		  targetPlayer(asShort(stmt, C::target_player)), targetId(asShort(stmt, C::target_id)),
		  targetValue(asFloat(stmt, C::target_value)), timeTicks(asUI(stmt, C::time_ticks)) {
		assert(magic_enum::enum_cast<MissionGoalType>(asText(stmt, C::type)).has_value());
		assert(!textKey.Empty());
	}
};

struct db_map : db_with_name {
	const Urho3D::String xmlName;
	const std::vector<unsigned short> ageIds;
	std::vector<unsigned short> goalIds;
	std::vector<db_mission_goal*> goals;

	db_map(sqlite3_stmt* stmt)
		: db_with_name(asShort(stmt, MapCol::id), asText(stmt, MapCol::name)), xmlName(asText(stmt, MapCol::xmlName)),
		  ageIds(parseIds(asText(stmt, MapCol::age_ids))), goalIds(parseIds(asText(stmt, MapCol::goal_ids))) {}

	private:
	static std::vector<unsigned short> parseIds(const char* value) {
		std::vector<unsigned short> result;
		if (!value) {
			return result;
		}
		std::string_view input(value);
		if (input.empty()) return result;
		while (!input.empty()) {
			const auto comma = input.find(',');
			const auto token = input.substr(0, comma);
			unsigned short id{};
			const auto [end, error] = std::from_chars(token.data(), token.data() + token.size(), id);
			if (error != std::errc{} || end != token.data() + token.size()) {
				return {};
			}
			result.push_back(id);
			if (comma == std::string_view::npos) {
				break;
			}
			input.remove_prefix(comma + 1);
		}
		return result;
	}
};

struct db_player_colors : db_with_name {
	const unsigned unit;
	const unsigned building;

	Urho3D::Color unitColor;
	Urho3D::Color buildingColor;
	using C = PlayerColorsCol;
	db_player_colors(sqlite3_stmt* s)
		: db_with_name(asShort(s, C::id), asText(s, C::name)),
		  unit(asHex(s, C::unit)),
		  building(asHex(s, C::building)),
		  unitColor(unit), buildingColor(building) {}
};
