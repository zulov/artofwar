#pragma once
#define UNITS_NUMBER_DB 50
#define BUILDINGS_NUMBER_DB 50
#define GRAPH_SETTINGS_NUMBER_DB 4
#define NATION_NUMBER_DB 10
#define RESOURCE_NUMBER_DB 4
#define ORDERS_NUMBER_DB 10
#define MAP_NUMBER_DB 10
#define PLAYER_COLORS_NUMBER_DB 10
#define RESOLUTIONS_NUMBER_DB 6
#define SETTINGS_NUMBER_DB 1
#define MAX_NUMBER_OF_NATIONS 5

#define SPLIT_SIGN '\n'

#include <optional>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Math/Vector2.h>
#include <utility>
#include <vector>
#include "utils.h"


struct db_order;
struct db_building_level;
struct db_cost;

struct db_unit_level {
	const int level;
	const int unit;
	const Urho3D::String name;
	const float minDist;
	const float maxSep;
	const Urho3D::String nodeName;
	const float mass;
	const float attack;
	const int attackSpeed;
	const float attackRange;
	const float defense;
	const int maxHp;
	const float maxSpeed;
	const float minSpeed;
	const float collectSpeed;
	const float upgradeSpeed;
	const float maxForce;

	std::vector<db_cost*> costs;

	db_unit_level(int level, int unit, char* name, float minDist, float maxSep, char* nodeName,
	              float mass, float attack, int attackSpeed, float attackRange, float defense,
	              int maxHp, float maxSpeed, float minSpeed, float collectSpeed, float upgradeSpeed, float maxForce)
		: level(level),
		  unit(unit),
		  name(name),
		  minDist(minDist),
		  maxSep(maxSep),
		  nodeName(nodeName),
		  mass(mass),
		  attack(attack),
		  attackSpeed(attackSpeed),
		  attackRange(attackRange),
		  defense(defense),
		  maxHp(maxHp),
		  maxSpeed(maxSpeed),
		  minSpeed(minSpeed),
		  collectSpeed(collectSpeed),
		  upgradeSpeed(upgradeSpeed),
		  maxForce(maxForce) { }
};

struct db_unit {
	const int id;
	const Urho3D::String name;
	const bool rotatable;
	const int nation;
	const Urho3D::String icon;
	const int actionState;

	std::vector<db_cost*> costs;
	std::vector<db_unit_level*> levels;

	std::vector<db_order*> orders;

	db_unit(int id, char* name, int rotatable, int nation, char* icon, int actionState)
		: id(id),
		  name(name),
		  rotatable(rotatable),
		  nation(nation),
		  icon(icon),
		  actionState(actionState) { }

	~db_unit() {
		clear_vector(costs);
		clear_vector(levels);
	}

	std::optional<db_unit_level*> getLevel(int level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}

};

struct db_building {
	const short id;
	const Urho3D::String name;
	const Urho3D::IntVector2 size;
	const int nation;
	const Urho3D::String icon;

	std::vector<db_unit*> units;
	std::vector<db_cost*> costs;
	std::vector<db_building_level*> levels;

	db_building(int id, char* name, int sizeX, int sizeZ, int nation, char* icon)
		: id(id),
		  name(name),
		  size({sizeX, sizeZ}),
		  nation(nation),
		  icon(icon) { }

	~db_building() {
		clear_vector(units);
		clear_vector(costs);
		clear_vector(levels);
	}

	std::optional<db_building_level*> getLevel(int level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}
};

struct db_building_level {
	const int level;
	const int unit;
	const Urho3D::String name;
	const Urho3D::String nodeName;
	const short queueMaxCapacity;

	std::vector<db_cost*> costs;

	db_building_level(int level, int unit, char* name, char* nodeName, short queueMaxCapacity)
		: level(level),
		  unit(unit),
		  name(name),
		  nodeName(nodeName),
		  queueMaxCapacity(queueMaxCapacity) { }

	~db_building_level() {
		clear_vector(costs);
	}
};

struct db_hud_size {
	const int id;
	const Urho3D::String name;

	db_hud_size(int id, char* name)
		: id(id),
		  name(name) { }
};

struct db_settings {
	int graph;
	int resolution;

	db_settings(int graph, int resolution)
		: graph(graph),
		  resolution(resolution) { }
};

struct db_resolution {
	const int id;
	const int x;
	const int y;

	db_resolution(int id, int x, int y)
		: id(id),
		  x(x),
		  y(y) { }
};

struct db_graph_settings {
	int id;
	int hud_size;
	float max_fps;
	float min_fps;
	Urho3D::Vector<Urho3D::String> styles;
	Urho3D::String name;
	int texture_quality;
	bool fullscreen;
	bool v_sync;
	bool shadow;

	db_graph_settings(int id, int hudSize, char* styles, int fullscreen, float maxFps, float minFps,
	                  char* name, bool v_sync, bool shadow, int texture_quality)
		: id(id),
		  hud_size(hudSize),
		  max_fps(maxFps),
		  min_fps(minFps),
		  styles(Urho3D::String(styles).Split(SPLIT_SIGN)),
		  name(name),
		  texture_quality(texture_quality),
		  fullscreen(fullscreen),
		  v_sync(v_sync),
		  shadow(shadow) { }
};

struct db_nation {
	const int id;
	const Urho3D::String name;

	std::vector<db_unit*> units;
	std::vector<db_building*> buildings;

	db_nation(int id, char* name)
		: id(id),
		  name(name) { }
};

struct db_resource {
	const int id;
	const Urho3D::String name;
	const Urho3D::String icon;
	const int maxCapacity;
	Urho3D::Vector<Urho3D::String> nodeName;
	const Urho3D::IntVector2 size;
	const int maxUsers;
	const unsigned mini_map_color;

	db_resource(int id, char* name, char* icon, int maxCapacity, char* nodeName, int sizeX,
	            int sizeZ, int maxUsers, unsigned mini_map_color)
		: id(id),
		  name(name),
		  icon(icon),
		  maxCapacity(maxCapacity),
		  nodeName(Urho3D::String(nodeName).Split(SPLIT_SIGN)),
		  size(Urho3D::IntVector2(sizeX, sizeZ)),
		  maxUsers(maxUsers),
		  mini_map_color(mini_map_color) { }
};

struct db_hud_vars {
	const int id;
	const int hud_size;
	float value;
	const Urho3D::String name;

	db_hud_vars(int id, int hudSize, char* name, float value)
		: id(id),
		  hud_size(hudSize),
		  value(value),
		  name(name) { }
};

struct db_cost {
	const int id;
	const int resource;
	const int value;
	const int thing;
	const Urho3D::String resourceName;

	db_cost(int id, int resource, int value, Urho3D::String resourceName, int thing)
		: id(id),
		  resource(resource),
		  value(value),
		  thing(thing),
		  resourceName(std::move(resourceName)) { }
};

struct db_order {
	const int id;
	const Urho3D::String icon;
	const Urho3D::String name;

	db_order(int id, char* icon, char* name)
		: id(id),
		  icon(icon),
		  name(name) { }
};

struct db_map {
	const int id;
	const Urho3D::String height_map;
	const Urho3D::String texture;
	const float scale_hor;
	const float scale_ver;
	const Urho3D::String name;

	db_map(int id, char* heightMap, char* texture, float scaleHor, float scaleVer, char* name)
		: id(id),
		  height_map(heightMap),
		  texture(texture),
		  scale_hor(scaleHor),
		  scale_ver(scaleVer),
		  name(name) { }
};

struct db_player_colors {
	const int id;
	const unsigned unit;
	const unsigned building;
	const Urho3D::String name;

	db_player_colors(int id, unsigned unit, unsigned building, char* name)
		: id(id),
		  unit(unit),
		  building(building),
		  name(name) { }
};

struct db_container {
	std::vector<db_hud_size*> hudSizes;
	std::vector<db_hud_vars*> hudVars;
	db_graph_settings* graphSettings[GRAPH_SETTINGS_NUMBER_DB] = {nullptr};
	db_resolution* resolutions[RESOLUTIONS_NUMBER_DB] = {nullptr};
	db_settings* settings[SETTINGS_NUMBER_DB] = {nullptr};

	db_unit* units[UNITS_NUMBER_DB] = {nullptr};
	db_building* buildings[BUILDINGS_NUMBER_DB] = {nullptr};
	db_nation* nations[NATION_NUMBER_DB] = {nullptr};
	db_resource* resources[RESOURCE_NUMBER_DB] = {nullptr};
	db_order* orders[ORDERS_NUMBER_DB] = {nullptr};
	db_map* maps[MAP_NUMBER_DB] = {nullptr};
	db_player_colors* playerColors[PLAYER_COLORS_NUMBER_DB] = {nullptr};

	unsigned short graph_settings_size = 0;
	unsigned short unit_type_size = 0;
	unsigned short resource_size = 0;
	unsigned short nation_size = 0;
	unsigned short maps_size = 0;
	unsigned short player_colors_size = 0;


	explicit db_container() { }

	~db_container() {
		clear_vector(hudSizes);
		clear_vector(hudVars);

		clear_array(graphSettings, GRAPH_SETTINGS_NUMBER_DB);
		clear_array(resolutions, RESOLUTIONS_NUMBER_DB);
		clear_array(settings, SETTINGS_NUMBER_DB);
		clear_array(maps, MAP_NUMBER_DB);
		clear_array(resources, RESOURCE_NUMBER_DB);
		clear_array(nations, NATION_NUMBER_DB);
		clear_array(units, UNITS_NUMBER_DB);
		clear_array(orders, ORDERS_NUMBER_DB);
		clear_array(buildings, BUILDINGS_NUMBER_DB);
		clear_array(playerColors, PLAYER_COLORS_NUMBER_DB);
	}
};
