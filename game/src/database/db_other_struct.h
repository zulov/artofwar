#pragma once
#include <charconv>
#include <string_view>
#include <system_error>
#include <vector>

#include <Urho3D/Math/Color.h>

#include "db_basic_struct.h"
#include "db_columns.h"
#include "db_utils.h"

struct db_map : db_with_name {
	const Urho3D::String xmlName;
	const std::vector<unsigned short> ageIds;

	db_map(sqlite3_stmt* stmt)
		: db_with_name(asShort(stmt, MapCol::id), asText(stmt, MapCol::name)), xmlName(asText(stmt, MapCol::xml_name)),
		  ageIds(parseAgeIds(asText(stmt, MapCol::age_ids))) {}

	private:
	static std::vector<unsigned short> parseAgeIds(const char* value) {
		std::vector<unsigned short> result;
		if (!value) {
			return result;
		}
		std::string_view input(value);
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
