#pragma once
#define UNITS_NUMBER_DB 50
#define BULDINGS_NUMBER_DB 50
#define HUD_SIZES_NUMBER_DB 4
#define GRAPH_SETTINGS_NUMBER_DB 4
#include <Urho3D/Container/Str.h>

struct db_unit
{
	Urho3D::String name;
	double minDist;
	double maxSep;
	int type;
	Urho3D::String model;
	Urho3D::String texture;
	Urho3D::String font;
	double mass;
	double maxSpeed;
	double scale;

	db_unit(char* name, double minDist, double maxSep, int type, char* model, char* texture, char* font, double mass, double maxSpeed, double scale)
		: name(name),
		minDist(minDist),
		maxSep(maxSep),
		type(type),
		model(model),
		texture(texture),
		font(font),
		mass(mass),
		maxSpeed(maxSpeed),
		scale(scale) {
	}

	db_unit() {
	};
};

struct db_building
{
	Urho3D::String name;
	double minDist;
	int type;
	Urho3D::String model;
	Urho3D::String texture;
	Urho3D::String font;
	double scale;

	db_building() {
	};
};

struct db_hud_size
{
	int id;
	Urho3D::String name;
	int icon_size_x;
	int icon_size_y;

	db_hud_size(int id, char* name, int iconSizeX, int iconSizeY)
		: id(id),
		name(name),
		icon_size_x(iconSizeX),
		icon_size_y(iconSizeY) {
	}
};

struct db_graph_settings
{
	int id;
	int hud_size;
	int res_x;
	int res_y;
	Urho3D::String style;
	bool fullscreen;
	double max_fps;
	double min_fps;

	db_graph_settings(int id, int hudSize, int resX, int resY, char* style, int fullscreen, double maxFps, double minFps)
		: id(id),
		hud_size(hudSize),
		res_x(resX),
		res_y(resY),
		style(style),
		fullscreen(fullscreen),
		max_fps(maxFps),
		min_fps(minFps) {
	}
};

struct db_container
{
	db_unit* units[UNITS_NUMBER_DB];
	db_building* buildings[BULDINGS_NUMBER_DB];
	db_hud_size* hudSizes[HUD_SIZES_NUMBER_DB];
	db_graph_settings* graphSettings[GRAPH_SETTINGS_NUMBER_DB];
};
