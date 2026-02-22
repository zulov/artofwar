#pragma once
#include <Urho3D/Math/Color.h>

#include "db_basic_struct.h"
#include "db_utils.h"

struct db_map : db_with_name {
	const Urho3D::String xmlName;

	db_map(sqlite3_stmt* stmt) : db_with_name(asShort(stmt, 0), asText(stmt, 2)), xmlName(asText(stmt, 1)) {}
};

struct db_player_colors : db_with_name {
	const unsigned unit;
	const unsigned building;

	Urho3D::Color unitColor;
	Urho3D::Color buildingColor;
	db_player_colors(sqlite3_stmt* stmt):
	db_player_colors(short id, unsigned unit, unsigned building, char* name)
		: db_with_name(id, name),
		  unit(unit), building(building),
		  unitColor(unit), buildingColor(building) {}
};
