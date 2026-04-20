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
	db_player_colors(sqlite3_stmt* stmt) :db_player_colors(asShort(stmt, PlayerColorsCol::id), asHex(stmt, PlayerColorsCol::unit), asHex(stmt, PlayerColorsCol::building), asText(stmt, PlayerColorsCol::name)){}
	db_player_colors(short id, unsigned unit, unsigned building, const char* name)
		: db_with_name(id, name),
		  unit(unit), building(building),
		  unitColor(unit), buildingColor(building) {}
};
