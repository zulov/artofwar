#pragma once
#include "db_basic_struct.h"
#include "db_utils.h"

//TODO dla prostych to jest dobry pomys³ ale dla d³ugich chyba lepiej konstruktor zwyk³y
struct db_hud_size : db_with_name {
	db_hud_size(sqlite3_stmt* stmt) : db_with_name(asShort(stmt, 0), asText(stmt, 1)) {}
};

struct db_settings {
	short graph;
	short resolution;

	db_settings(sqlite3_stmt* stmt) : db_settings(asShort(stmt, 0), asShort(stmt, 1)) {}
	db_settings(short graph, short resolution) : graph(graph), resolution(resolution) {}
};

struct db_resolution : db_entity {
	const short x;
	const short y;

	db_resolution(sqlite3_stmt* stmt) : db_entity(asShort(stmt, 0)), x(asShort(stmt, 1)), y(asShort(stmt, 2)) {}
};

struct db_graph_settings : db_entity {
	short hud_size;
	float max_fps;
	float min_fps;
	Urho3D::Vector<Urho3D::String> styles;
	Urho3D::String name;
	short texture_quality;
	bool fullscreen;
	bool v_sync;
	bool shadow;

	db_graph_settings(sqlite3_stmt* stmt) : db_graph_settings(asShort(stmt, 0), asShort(stmt, 1), asText(stmt, 2),
	                                                          asInt(stmt, 3), asFloat(stmt, 4), asFloat(stmt, 5),
	                                                          asText(stmt, 6), asBool(stmt, 7), asBool(stmt, 8), asShort(stmt, 9)
	) {}

	db_graph_settings(short id, short hudSize, const char* styles, int fullscreen, float maxFps, float minFps,
	                  const char* name, bool v_sync, bool shadow, short texture_quality)
		: db_entity(id),
		  hud_size(hudSize),
		  max_fps(maxFps),
		  min_fps(minFps),
		  styles(Urho3D::String(styles).Split(SPLIT_SIGN)),
		  name(name),
		  texture_quality(texture_quality),
		  fullscreen(fullscreen),
		  v_sync(v_sync),
		  shadow(shadow) {}
};

struct db_hud_vars : db_with_name {
	const short hud_size;
	float value;

	db_hud_vars(sqlite3_stmt* stmt)
		: db_with_name(asShort(stmt, 0), asText(stmt, 2)), hud_size(asShort(stmt, 1)), value(asFloat(stmt, 3)) {}
};
