#pragma once

#define SPLIT_SIGN '\n'

#include <optional>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Math/Vector2.h>
#include <vector>
#include "objects/unit/state/UnitState.h"
#include "utils.h"

struct db_nation;
struct db_order;
struct db_building_level;
struct db_cost;

struct db_entity {
	const short id;

	explicit db_entity(short id)
		: id(id) { }
};

struct db_ai_property {
	float sum;

	float econ;
	float attack;
	float defence;

	db_ai_property(float econ, float attack, float defence)
		: sum(econ + attack + defence), //TODO bug div zero
		  econ(econ / sum),
		  attack(attack / sum),
		  defence(defence / sum) { }
};

struct db_ai_prop_level {
	db_ai_property* aiProps = nullptr;
	db_ai_property* aiPropsLevelUp = nullptr;

	~db_ai_prop_level() {
		delete aiProps;
		delete aiPropsLevelUp;
	}
};

template <typename T>
static void setEntity(std::vector<T*>& array, T* entity) {
	auto id = static_cast<db_entity*>(entity)->id;

	if (array.size() <= id) {
		array.resize(id + 1, nullptr);
	}

	array[id] = entity;
}


struct db_unit_level : db_entity, db_ai_prop_level {
	const short level;
	const short unit;
	const Urho3D::String name;
	const float minDist;
	const float maxSep;
	const Urho3D::String nodeName;
	const float mass;
	const float attack;
	const short attackSpeed;
	const float attackRange;
	const float defense;
	const short maxHp;
	const float maxSpeed;
	const float minSpeed;
	const float collectSpeed;
	const float upgradeSpeed;
	const float maxForce;

	std::vector<db_cost*> costs;
	
	db_unit_level(short id, short level, short unit, char* name, float minDist, float maxSep, char* nodeName,
	              float mass, float attack, short attackSpeed, float attackRange, float defense,
	              short maxHp, float maxSpeed, float minSpeed, float collectSpeed, float upgradeSpeed, float maxForce)
		: db_entity(id),
		  level(level),
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

	~db_unit_level() {
		clear_vector(costs);
	}
};

struct db_unit : db_entity {

	const Urho3D::String name;
	const bool rotatable;
	const Urho3D::String icon;
	const short actionState;

	std::vector<db_cost*> costs;
	std::vector<db_unit_level*> levels;

	std::vector<db_order*> orders;
	std::vector<db_nation*> nations;

	bool possibleStates[STATE_SIZE];

	db_unit(int id, char* name, int rotatable, char* icon, int actionState)
		: db_entity(id),
		  name(name),
		  rotatable(rotatable),
		  icon(icon),
		  actionState(actionState) { }

	~db_unit() {
		clear_vector(costs);
	}

	std::optional<db_unit_level*> getLevel(int level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}

};

struct db_building : db_entity {

	const Urho3D::String name;
	const Urho3D::IntVector2 size;
	const Urho3D::String icon;

	std::vector<db_cost*> costs;
	std::vector<db_building_level*> levels;

	std::vector<db_nation*> nations;

	db_building(int id, char* name, int sizeX, int sizeZ, char* icon)
		: db_entity(id),
		  name(name),
		  size({sizeX, sizeZ}),
		  icon(icon) { }

	~db_building() {
		clear_vector(costs);
	}

	std::optional<db_building_level*> getLevel(int level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}
};

struct db_building_level : db_entity, db_ai_prop_level {
	const short level;
	const short building;
	const Urho3D::String name;
	const Urho3D::String nodeName;
	const short queueMaxCapacity;

	std::vector<db_cost*> costs;
	std::vector<db_unit*> allUnits;

	//std::vector<std::vector<db_unit*>> unitsPerNation;

	db_building_level(short id, short level, short building, char* name, char* nodeName, short queueMaxCapacity)
		: db_entity(id),
		  level(level),
		  building(building),
		  name(name),
		  nodeName(nodeName),
		  queueMaxCapacity(queueMaxCapacity) { }

	~db_building_level() {
		clear_vector(costs);

	}
};

struct db_hud_size : public db_entity {
	const Urho3D::String name;

	db_hud_size(short id, char* name)
		: db_entity(id),
		  name(name) { }
};

struct db_settings {
	short graph;
	short resolution;

	db_settings(short graph, short resolution)
		: graph(graph),
		  resolution(resolution) { }
};

struct db_resolution : public db_entity {
	const short x;
	const short y;

	db_resolution(short id, short x, short y)
		: db_entity(id),
		  x(x),
		  y(y) { }
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

	db_graph_settings(short id, short hudSize, char* styles, int fullscreen, float maxFps, float minFps,
	                  char* name, bool v_sync, bool shadow, short texture_quality)
		: db_entity(id),
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

struct db_nation : db_entity {
	const Urho3D::String name;

	std::vector<db_unit*> units;
	std::vector<db_building*> buildings;

	db_nation(int id, char* name)
		: db_entity(id),
		  name(name) { }
};

struct db_resource : db_entity {
	const Urho3D::String name;
	const Urho3D::String icon;
	const short maxCapacity;
	Urho3D::Vector<Urho3D::String> nodeName;
	const Urho3D::IntVector2 size;
	const short maxUsers;
	const unsigned mini_map_color;

	db_resource(short id, char* name, char* icon, int maxCapacity, char* nodeName, short sizeX,
	            short sizeZ, short maxUsers, unsigned mini_map_color)
		: db_entity(id),
		  name(name),
		  icon(icon),
		  maxCapacity(maxCapacity),
		  nodeName(Urho3D::String(nodeName).Split(SPLIT_SIGN)),
		  size(Urho3D::IntVector2(sizeX, sizeZ)),
		  maxUsers(maxUsers),
		  mini_map_color(mini_map_color) { }
};

struct db_hud_vars : db_entity {
	const short hud_size;
	float value;
	const Urho3D::String name;

	db_hud_vars(short id, short hudSize, char* name, float value)
		: db_entity(id),
		  hud_size(hudSize),
		  value(value),
		  name(name) { }
};

struct db_cost {
	const short resource;
	const short value;

	db_cost(short resource, short value)
		: resource(resource),
		  value(value) { }
};

struct db_order : db_entity {
	const Urho3D::String icon;
	const Urho3D::String name;

	db_order(int id, char* icon, char* name)
		: db_entity(id),
		  icon(icon),
		  name(name) { }
};

struct db_map : db_entity {
	const Urho3D::String height_map;
	const Urho3D::String texture;
	const float scale_hor;
	const float scale_ver;
	const Urho3D::String name;

	db_map(short id, char* heightMap, char* texture, float scaleHor, float scaleVer, char* name)
		: db_entity(id),
		  height_map(heightMap),
		  texture(texture),
		  scale_hor(scaleHor),
		  scale_ver(scaleVer),
		  name(name) { }
};

struct db_player_colors : db_entity {
	const unsigned unit;
	const unsigned building;
	const Urho3D::String name;

	db_player_colors(int id, unsigned unit, unsigned building, char* name)
		: db_entity(id),
		  unit(unit),
		  building(building),
		  name(name) { }
};

struct db_container {
	std::vector<db_hud_size*> hudSizes;
	std::vector<db_hud_vars*> hudVars;
	std::vector<db_graph_settings*> graphSettings;
	std::vector<db_resolution*> resolutions;
	std::vector<db_settings*> settings;

	std::vector<db_map*> maps;

	std::vector<db_unit*> units;
	std::vector<db_unit_level*> unitsLevels;
	std::vector<db_building*> buildings;
	std::vector<db_building_level*> buildingsLevels;
	std::vector<db_nation*> nations;
	std::vector<db_resource*> resources;
	std::vector<db_player_colors*> playerColors;
	std::vector<db_order*> orders;

	explicit db_container() = default;

	~db_container() {
		clear_vector(hudSizes);
		clear_vector(hudVars);
		clear_vector(resolutions);
		clear_vector(graphSettings);
		clear_vector(settings);

		clear_vector(maps);

		clear_vector(resources);
		clear_vector(nations);
		clear_vector(units);
		clear_vector(orders);
		clear_vector(buildings);
		clear_vector(playerColors);
		clear_vector(unitsLevels);
		clear_vector(buildingsLevels);
	}
};
