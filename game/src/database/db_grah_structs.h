#pragma once
#include "db_basic_struct.h"
#include "db_columns.h"
#include "db_utils.h"

//TODO dla prostych to jest dobry pomys� ale dla d�ugich chyba lepiej konstruktor zwyk�y
struct db_hud_size : db_with_name {
	db_hud_size(sqlite3_stmt* stmt) : db_with_name(asShort(stmt, HudSizeCol::id), asText(stmt, HudSizeCol::name)) {}
};

struct db_settings {
	short graph;
	short resolution;

	db_settings(sqlite3_stmt* s) : graph(asShort(s, SettingsCol::graph)), resolution(asShort(s, SettingsCol::resolution)) {}
	db_settings(short graph, short resolution) : graph(graph), resolution(resolution) {}
};

struct db_resolution : db_entity {
	const short x;
	const short y;

	db_resolution(sqlite3_stmt* stmt) : db_entity(asShort(stmt, ResolutionCol::id)), x(asShort(stmt, ResolutionCol::x)), y(asShort(stmt, ResolutionCol::y)) {}
};

struct db_graph_settings : db_entity {
	short hud_size;
	int max_fps;
	int min_fps;
	Urho3D::Vector<Urho3D::String> styles;
	Urho3D::String name;
	short texture_quality;
	bool fullscreen;
	bool v_sync;
	bool shadow;

	using C = GraphSettingsCol;
	db_graph_settings(sqlite3_stmt* s)
		: db_entity(asShort(s, C::id)),
		  hud_size(asShort(s, C::hud_size)),
		  max_fps(asInt(s, C::max_fps)),
		  min_fps(asInt(s, C::min_fps)),
		  styles(Urho3D::String(asText(s, C::styles)).Split(SPLIT_SIGN)),
		  name(asText(s, C::name)),
		  texture_quality(asShort(s, C::texture_quality)),
		  fullscreen(asInt(s, C::fullscreen)),
		  v_sync(asBool(s, C::v_sync)),
		  shadow(asBool(s, C::shadow)) {}

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
		: db_with_name(asShort(stmt, HudVarsCol::id), asText(stmt, HudVarsCol::name)), hud_size(asShort(stmt, HudVarsCol::hud_size)), value(asFloat(stmt, HudVarsCol::value)) {}
};
