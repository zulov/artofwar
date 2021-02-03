#pragma once
#include "db_basic_struct.h"

struct db_order : db_entity, db_with_name {
	const Urho3D::String icon;

	db_order(short id, char* icon, char* name)
		: db_entity(id), db_with_name(name),
		  icon(icon) {
	}
};

struct db_map : db_entity, db_with_name {
	const Urho3D::String xmlName;

	db_map(short id, char* xmlName, char* name): db_entity(id), db_with_name(name), xmlName(xmlName) {
	}
};

struct db_player_colors : db_entity, db_with_name {
	const unsigned unit;
	const unsigned building;

	db_player_colors(short id, unsigned unit, unsigned building, char* name)
		: db_entity(id), db_with_name(name),
		  unit(unit),
		  building(building) {
	}
};
