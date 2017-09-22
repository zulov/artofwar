#pragma once
#define UNITS_NUMBER_DB 50
#define BULDINGS_NUMBER_DB 50
#define HUD_SIZES_NUMBER_DB 4
#define GRAPH_SETTINGS_NUMBER_DB 4
#define TYPE_NUMBER_DB 50
#define NATION_NUMBER_DB 50
#define RESOURCE_NUMBER_DB 10
#define HUD_VARS_NUMBER_DB 50
#define ORDERS_NUMBER_DB 20

#define SPLIT_SIGN '\n'

#include <Urho3D/Container/Str.h>
#include <vector>
#include "defines.h"

struct db_unit
{
	int id;
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
	int nation;
	Urho3D::String icon;

	db_unit(int id, char* name, double minDist, double maxSep, int type, char* model, char* texture, char* font,
	        double mass, double maxSpeed, double scale, int rotatable, int nation, char* icon)
		: id(id),
		name(name),
		minDist(minDist),
		maxSep(maxSep),
		type(type),
		model(model),
		texture(texture),
		font(font),
		mass(mass),
		maxSpeed(maxSpeed),
		scale(scale),
		rotatable(rotatable),
		nation(nation),
		icon(icon) {
	}
};

struct db_building
{
	int id;
	Urho3D::String name;
	double minDist;
	int type;
	Urho3D::String model;
	Urho3D::String texture;
	Urho3D::String font;
	double scale;
	Urho3D::String texture_temp;
	int nation;
	Urho3D::String icon;
	short queueMaxCapacity;

	db_building(int id, char* name, double minDist, int type, char* model, char* texture, char* font, double scale,
	            char* texture_temp, int nation, char* icon, int queueMaxCapacity)
		: id(id),
		name(name),
		minDist(minDist),
		type(type),
		model(model),
		texture(texture),
		font(font),
		scale(scale),
		texture_temp(texture_temp),
		nation(nation),
		icon(icon),
		queueMaxCapacity(queueMaxCapacity) {
	}
};

struct db_hud_size
{
	int id;
	Urho3D::String name;

	db_hud_size(int id, char* name)
		: id(id),
		name(name) {
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

	db_unit_type(int id, char* name, char* icon)
		: id(id),
		name(name) {
	}
};

struct db_building_type
{
	int id;
	Urho3D::String name;

	db_building_type(int id, char* name)
		: id(id),
		name(name) {
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
	int maxUsers;

	db_resource(int id, char* name, char* icon, int maxCapacity, char* texture, char* model, double scale, double minDist,
	            int maxUsers)
		: id(id),
		name(name),
		icon(icon),
		maxCapacity(maxCapacity),
		texture(Urho3D::String(texture).Split(SPLIT_SIGN)),
		model(model),
		scale(scale),
		minDist(minDist),
		maxUsers(maxUsers) {
	}
};

struct db_hud_vars
{
	int id;
	int hud_size;
	Urho3D::String name;
	double value;

	db_hud_vars(int id, int hudSize, char* name, double value)
		: id(id),
		hud_size(hudSize),
		name(name),
		value(value) {
	}
};

struct db_cost
{
	int id;
	int resource;
	int value;
	Urho3D::String resourceName;
	int thing;

	db_cost(int id, int resource, int value, Urho3D::String resourceName, int thing)
		: id(id),
		resource(resource),
		value(value),
		resourceName(resourceName),
		thing(thing) {
	}
};

struct db_order
{
	int id;
	Urho3D::String icon;


	db_order(int id, char* icon)
		: id(id),
		icon(icon) {
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
	db_hud_vars* hudVars[HUD_VARS_NUMBER_DB];
	std::vector<db_unit*>* unitsForBuilding[BULDINGS_NUMBER_DB];
	std::vector<db_cost*>* costForBuilding[BULDINGS_NUMBER_DB];
	std::vector<db_cost*>* costForUnit[UNITS_NUMBER_DB];
	std::vector<db_order*>* ordersToUnit[UNITS_NUMBER_DB];
	db_order* orders[ORDERS_NUMBER_DB];

	int units_size = 0;
	int hud_size_size = 0;
	int building_size = 0;
	int graph_settings_size = 0;
	int unit_type_size = 0;
	int building_type_size = 0;
	int resource_size = 0;
	int nation_size = 0;
	int hud_vars_size = 0;
	int orders_size = 0;


	explicit db_container() {
		for (int i = 0; i < BULDINGS_NUMBER_DB; ++i) {
			unitsForBuilding[i] = new std::vector<db_unit*>();
			unitsForBuilding[i]->reserve(DEFAULT_VECTOR_SIZE);
			costForBuilding[i] = new std::vector<db_cost*>();
			costForBuilding[i]->reserve(DEFAULT_VECTOR_SIZE);
		}
		for (int i = 0; i < UNITS_NUMBER_DB; ++i) {
			costForUnit[i] = new std::vector<db_cost*>();
			costForUnit[i]->reserve(DEFAULT_VECTOR_SIZE);
			ordersToUnit[i] = new std::vector<db_order*>();
			ordersToUnit[i]->reserve(DEFAULT_VECTOR_SIZE);
		}
	}
};
