#pragma once
#define UNITS_NUMBER_DB 50
#define BULDINGS_NUMBER_DB 50
#define HUD_SIZES_NUMBER_DB 4
#define GRAPH_SETTINGS_NUMBER_DB 4
#define TYPE_NUMBER_DB 50
#define NATION_NUMBER_DB 50
#define RESOURCE_NUMBER_DB 4

#define SPLIT_SIGN '\n'

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
	bool rotatable;

	db_unit(char* name, double minDist, double maxSep, int type, char* model, char* texture, char* font, double mass, double maxSpeed, double scale, int rotatable)
		: name(name),
		minDist(minDist),
		maxSep(maxSep),
		type(type),
		model(model),
		texture(texture),
		font(font),
		mass(mass),
		maxSpeed(maxSpeed),
		scale(scale),
		rotatable(rotatable) {
	}
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
	Urho3D::String texture_temp;

	db_building(char* name, double minDist, int type, char* model, char* texture, char* font, double scale, char* texture_temp)
		: name(name),
		minDist(minDist),
		type(type),
		model(model),
		texture(texture),
		font(font),
		scale(scale),
		texture_temp(texture_temp) {
	}
};

struct db_hud_size
{
	int id;
	Urho3D::String name;
	int icon_size_x;
	int icon_size_y;
	int space_size_x;
	int space_size_y;
	Urho3D::Vector<Urho3D::String> names;
	Urho3D::Vector<Urho3D::String> values;

	db_hud_size(int id, char* name, int iconSizeX, int iconSizeY, int spaceSizeX, int spaceSizeY, char* names, char* values)
		: id(id),
		name(name),
		icon_size_x(iconSizeX),
		icon_size_y(iconSizeY),
		space_size_x(spaceSizeX),
		space_size_y(spaceSizeY),
		names(Urho3D::String(names).Split(SPLIT_SIGN)),
		values(Urho3D::String(values).Split(SPLIT_SIGN)){
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

struct db_unit_type
{
	int id;
	Urho3D::String name;
	Urho3D::String icon;

	db_unit_type(int id, char* name, char* icon)
		: id(id),
		name(name),
		icon(icon) {
	}
};

struct db_building_type
{
	int id;
	Urho3D::String name;
	Urho3D::String icon;

	db_building_type(int id, char* name, char* icon)
		: id(id),
		name(name),
		icon(icon) {
	}
};

struct db_nation
{
	int id;
	Urho3D::String name;

	db_nation(int id, char* name)
		: id(id),
		name(name) {
	}
};

struct db_resource
{
	int id;
	Urho3D::String name;
	Urho3D::String icon;
	int maxCapacity;
	Urho3D::Vector<Urho3D::String> texture;
	Urho3D::String model;
	double scale;
	double minDist;


	db_resource(int id, char* name, char* icon, int maxCapacity, char* texture, char* model, double scale, double minDist)
		: id(id),
		name(name),
		icon(icon),
		maxCapacity(maxCapacity),
		texture(Urho3D::String(texture).Split(SPLIT_SIGN)),
		model(model),
		scale(scale),
		minDist(minDist) {
	}
};

struct db_container
{
	db_unit* units[UNITS_NUMBER_DB];
	db_building* buildings[BULDINGS_NUMBER_DB];
	db_hud_size* hudSizes[HUD_SIZES_NUMBER_DB];
	db_graph_settings* graphSettings[GRAPH_SETTINGS_NUMBER_DB];
	db_unit_type* unitTypes[TYPE_NUMBER_DB];
	db_building_type* buildingTypes[TYPE_NUMBER_DB];
	db_nation* nations[NATION_NUMBER_DB];
	db_resource* resources[RESOURCE_NUMBER_DB];

	int units_size = 0;
	int hud_size_size = 0;
	int building_size = 0;
	int graph_settings_size = 0;
	int unit_type_size = 0;
	int building_type_size = 0;
	int resource_size = 0;
	int nation_size = 0;
};
