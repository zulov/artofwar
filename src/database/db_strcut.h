#pragma once
#define UNITS_NUMBER_DB 50
#define BUILDINGS_NUMBER_DB 50
#define HUD_SIZES_NUMBER_DB 4
#define GRAPH_SETTINGS_NUMBER_DB 4
#define TYPE_NUMBER_DB 50
#define NATION_NUMBER_DB 50
#define RESOURCE_NUMBER_DB 5
#define HUD_VARS_NUMBER_DB 50
#define ORDERS_NUMBER_DB 20
#define MAP_NUMBER_DB 10
#define PLAYER_COLORS_NUMBER_DB 10
#define RESOLUTIONS_NUMBER_DB 6
#define SETTINGS_NUMBER_DB 1

#define SPLIT_SIGN '\n'

#include "defines.h"
#include <Urho3D/Container/Str.h>
#include <Urho3D/Math/Vector2.h>
#include <vector>


struct db_unit
{
	int id;
	Urho3D::String name;
	bool rotatable;
	int nation;
	Urho3D::String icon;
	int actionState;

	db_unit(int id, char* name, int rotatable, int nation, char* icon, int actionState)
		: id(id),
		name(name),
		rotatable(rotatable),
		nation(nation),
		icon(icon),
		actionState(actionState) {
	}
};

struct db_unit_level
{
	int level;
	int unit;
	Urho3D::String name;
	float minDist;
	float maxSep;
	Urho3D::String model;
	Urho3D::String texture;
	float mass;
	float scale;
	float attack;
	int attackSpeed;
	float attackRange;
	float defense;
	int maxHp;
	float maxSpeed;
	float minSpeed;
	float collectSpeed;
	float upgradeSpeed;


	db_unit_level(int level, int unit, char* name, float minDist, float maxSep, char* model, char* texture,
	              float mass, float scale, float attack, int attackSpeed, float attackRange, float defense,
	              int maxHp, float maxSpeed, float minSpeed, float collectSpeed, float upgradeSpeed)
		: level(level),
		unit(unit),
		name(name),
		minDist(minDist),
		maxSep(maxSep),
		model(model),
		texture(texture),
		mass(mass),
		scale(scale),
		attack(attack),
		attackSpeed(attackSpeed),
		attackRange(attackRange),
		defense(defense),
		maxHp(maxHp),
		maxSpeed(maxSpeed),
		minSpeed(minSpeed),
		collectSpeed(collectSpeed),
		upgradeSpeed(upgradeSpeed) {
	}
};

struct db_building
{
	int id;
	Urho3D::String name;
	Urho3D::IntVector2 size;
	int sizeZ;
	Urho3D::String model;
	Urho3D::String texture;
	float scale;

	int nation;
	Urho3D::String icon;
	short queueMaxCapacity;

	db_building(int id, char* name, int sizeX, int sizeZ, char* model, char* texture, float scale,
	            int nation, char* icon, int queueMaxCapacity)
		: id(id),
		name(name),
		size(Urho3D::IntVector2(sizeX, sizeZ)),
		model(model),
		texture(texture),
		scale(scale),
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

struct db_settings
{
	int graph;
	int resolution;

	db_settings(int graph, int resolution)
		: graph(graph),
		resolution(resolution) {
	}
};

struct db_resolution
{
	int id;
	int x;
	int y;

	db_resolution(int id, int x, int y)
		: id(id),
		x(x),
		y(y) {
	}
};

struct db_graph_settings
{
	int id;
	int hud_size;
	Urho3D::Vector<Urho3D::String> styles;
	bool fullscreen;
	float max_fps;
	float min_fps;
	Urho3D::String name;
	bool v_sync;
	bool shadow;
	int texture_quality;

	db_graph_settings(int id, int hudSize, char* styles, int fullscreen, float maxFps, float minFps,
	                  char* name, bool v_sync, bool shadow, int texture_quality)
		: id(id),
		hud_size(hudSize),
		styles(Urho3D::String(styles).Split(SPLIT_SIGN)),
		fullscreen(fullscreen),
		max_fps(maxFps),
		min_fps(minFps),
		name(name),
		v_sync(v_sync),
		shadow(shadow),
		texture_quality(texture_quality) {
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
	float scale;
	Urho3D::IntVector2 size;
	int maxUsers;
	unsigned mini_map_color;

	db_resource(int id, char* name, char* icon, int maxCapacity, char* texture, char* model, float scale, int sizeX,
	            int sizeZ, int maxUsers, unsigned mini_map_color)
		: id(id),
		name(name),
		icon(icon),
		maxCapacity(maxCapacity),
		texture(Urho3D::String(texture).Split(SPLIT_SIGN)),
		model(model),
		scale(scale),
		size(Urho3D::IntVector2(sizeX, sizeZ)),
		maxUsers(maxUsers),
		mini_map_color(mini_map_color) {
	}
};

struct db_hud_vars
{
	int id;
	int hud_size;
	Urho3D::String name;
	float value;

	db_hud_vars(int id, int hudSize, char* name, float value)
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

struct db_map
{
	int id;
	Urho3D::String height_map;
	Urho3D::String texture;
	float scale_hor;
	float scale_ver;
	Urho3D::String name;

	db_map(int id, char* heightMap, char* texture, float scaleHor, float scaleVer, char* name)
		: id(id),
		height_map(heightMap),
		texture(texture),
		scale_hor(scaleHor),
		scale_ver(scaleVer),
		name(name) {
	}
};

struct db_player_colors
{
	int id;
	unsigned unit;
	unsigned building;
	Urho3D::String name;

	db_player_colors(int id, unsigned unit, unsigned building, char* name)
		: id(id),
		unit(unit),
		building(building),
		name(name) {
	}
};

struct db_container
{
	db_unit* units[UNITS_NUMBER_DB] = {nullptr};
	db_building* buildings[BUILDINGS_NUMBER_DB] = {nullptr};
	db_hud_size* hudSizes[HUD_SIZES_NUMBER_DB] = {nullptr};
	db_graph_settings* graphSettings[GRAPH_SETTINGS_NUMBER_DB] = {nullptr};
	db_nation* nations[NATION_NUMBER_DB] = {nullptr};
	db_resource* resources[RESOURCE_NUMBER_DB] = {nullptr};
	db_hud_vars* hudVars[HUD_VARS_NUMBER_DB] = {nullptr};
	db_order* orders[ORDERS_NUMBER_DB] = {nullptr};
	db_map* maps[MAP_NUMBER_DB] = {nullptr};
	db_player_colors* playerColors[PLAYER_COLORS_NUMBER_DB] = {nullptr};
	db_resolution* resolutions[RESOLUTIONS_NUMBER_DB] = {nullptr};
	db_settings* settings[SETTINGS_NUMBER_DB] = {nullptr};

	std::vector<db_unit*>* unitsForBuilding[BUILDINGS_NUMBER_DB]{};
	std::vector<db_cost*>* costForBuilding[BUILDINGS_NUMBER_DB]{};
	std::vector<db_cost*>* costForUnit[UNITS_NUMBER_DB]{};
	std::vector<db_order*>* ordersToUnit[UNITS_NUMBER_DB]{};
	std::vector<db_unit_level*>* levelsToUnit[UNITS_NUMBER_DB]{};

	int units_size = 0;
	int hud_size_size = 0;
	int building_size = 0;
	int graph_settings_size = 0;
	int unit_type_size = 0;
	int resource_size = 0;
	int nation_size = 0;
	int hud_vars_size = 0;
	int orders_size = 0;
	int maps_size = 0;
	int player_colors_size = 0;
	int resolutions_size = 0;


	explicit db_container() {
		for (int i = 0; i < BUILDINGS_NUMBER_DB; ++i) {
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
			levelsToUnit[i] = new std::vector<db_unit_level*>();
			levelsToUnit[i]->reserve(DEFAULT_VECTOR_SIZE);
		}
	}

	~db_container() = default;
};
