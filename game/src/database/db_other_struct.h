#pragma once
#include <Urho3D/Math/Color.h>

#include "db_basic_struct.h"

struct db_order : db_with_icon {

	db_order(short id, char* icon, char* name) : db_with_icon(id, name, icon) {
	}
};

struct db_map : db_with_name {
	const Urho3D::String xmlName;

	db_map(short id, char* xmlName, char* name): db_with_name(id, name), xmlName(xmlName) {
	}
};

struct db_player_colors : db_with_name {
	const unsigned unit;
	const unsigned building;

	Urho3D::Color unitColor;
	Urho3D::Color buildingColor;

	db_player_colors(short id, unsigned unit, unsigned building, char* name)
		: db_with_name(id, name),
		  unit(unit), building(building),
		  unitColor(unit), buildingColor(building) {
	}
};
