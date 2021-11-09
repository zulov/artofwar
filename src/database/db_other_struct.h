#pragma once
#include <Urho3D/Math/Color.h>

#include "db_basic_struct.h"

struct db_order : db_entity, db_with_name {
	const Urho3D::String icon;

	db_order(short id, char* icon, char* name)
		: db_entity(id), db_with_name(name),
		  icon(icon) { }
};

struct db_map : db_entity, db_with_name {
	const Urho3D::String xmlName;

	db_map(short id, char* xmlName, char* name): db_entity(id), db_with_name(name), xmlName(xmlName) { }
};

struct db_player_colors : db_entity, db_with_name {
	const unsigned unit;
	const unsigned building;

	Urho3D::Color unitColor;
	Urho3D::Color buildingColor;

	float castToColor(unsigned char val) {
		return val / 255.f;
	}

	db_player_colors(short id, unsigned unit, unsigned building, char* name)
		: db_entity(id), db_with_name(name),
		  unit(unit),
		  building(building) {

		unitColor = Urho3D::Color(castToColor(unit), castToColor(unit >> 8), castToColor(unit >> 16),
		                          castToColor(unit >> 24));
		buildingColor = Urho3D::Color(castToColor(building), castToColor(building >> 8), castToColor(building >> 16),
		                              castToColor(building >> 24));
	}
};
