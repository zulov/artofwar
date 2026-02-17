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

	db_settings(short graph, short resolution)
		: graph(graph),
		  resolution(resolution) {}
};

struct db_resolution : db_entity {
	const short x;
	const short y;

	db_resolution(short id, short x, short y)
		: db_entity(id), x(x), y(y) {}
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

	db_hud_vars(short id, short hudSize, char* name, float value)
		: db_with_name(id, name), hud_size(hudSize), value(value) {}
};
