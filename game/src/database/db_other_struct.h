#pragma once
#include <Urho3D/Math/Color.h>

#include "db_basic_struct.h"
#include "db_columns.h"
#include "db_utils.h"

struct db_map : db_with_name {
	const Urho3D::String xmlName;

	db_map(sqlite3_stmt* stmt) : db_with_name(asShort(stmt, MapCol::id), asText(stmt, MapCol::name)), xmlName(asText(stmt, MapCol::xml_name)) {}
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
