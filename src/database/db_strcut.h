#pragma once

#define SPLIT_SIGN '\n'
#define AI_PROPS_SIZE 4

#include <optional>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Math/Vector2.h>
#include <utility>
#include <vector>
#include <string>
#include "objects/unit/state/UnitState.h"
#include "objects/ValueType.h"
#include "utils.h"


struct db_nation;
struct db_order;
struct db_building_level;
struct db_cost;

struct db_entity {
	const short id;

	explicit db_entity(short id)
		: id(id) {
	}
};

struct db_with_name {
	const Urho3D::String name;

	explicit db_with_name(const Urho3D::String& name)
		: name(name) {
	}
};

struct db_ai_property {
private:
	float costSum = 0;
	const float sum;
	std::string paramsNormAString;
public:
	float paramsNorm[AI_PROPS_SIZE];

	const float econ;
	const float attack;
	const float defence;

	db_ai_property(float econ, float attack, float defence)
		: sum(econ + attack + defence), //TODO bug div zero
		  econ(econ),
		  attack(attack),
		  defence(defence), paramsNorm{econ, attack, defence} {
	}

	const std::string& getParamsNormAsString() const { return paramsNormAString; }

	void setCostSum(float costSum, float normDiv) {
		this->costSum = costSum;
		paramsNorm[3] = costSum / normDiv;
	}

	void setParamsNormAString(std::string paramsNormAString) {
		this->paramsNormAString = std::move(paramsNormAString);
	}

	float getValueOf(ValueType type) {
		switch (type) {
		case ValueType::ATTACK: return attack;
		case ValueType::DEFENCE: return defence;
		case ValueType::ECON: return econ;
		}
	}
};

struct db_level {
	const short level;
	db_ai_property* aiProps = nullptr;
	db_ai_property* aiPropsLevelUp = nullptr;


	explicit db_level(short level)
		: level(level) {
	}

	~db_level() {
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


struct db_unit_level : db_entity, db_level, db_with_name {
	const short unit;
	const float minDist;
	const float maxSep;
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

	const Urho3D::String nodeName;

	std::vector<db_cost*> costs;

	db_unit_level(short id, short level, short unit, char* name, float minDist, float maxSep, char* nodeName,
	              float mass, float attack, short attackSpeed, float attackRange, float defense,
	              short maxHp, float maxSpeed, float minSpeed, float collectSpeed, float upgradeSpeed, float maxForce)
		: db_entity(id), db_level(level), db_with_name(name),
		  unit(unit),
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
		  maxForce(maxForce) {
	}

	~db_unit_level() {
		clear_vector(costs);
	}
};

struct db_unit : db_entity, db_with_name {
	const bool rotatable;
	const Urho3D::String icon;
	const short actionState;

	std::vector<db_cost*> costs;
	std::vector<db_unit_level*> levels;

	std::vector<db_order*> orders;
	std::vector<db_nation*> nations;

	bool possibleStates[STATE_SIZE];

	db_unit(short id, char* name, short rotatable, char* icon, short actionState)
		: db_entity(id), db_with_name(name),
		  rotatable(rotatable),
		  icon(icon),
		  actionState(actionState) {
	}

	~db_unit() {
		clear_vector(costs);
	}

	std::optional<db_unit_level*> getLevel(short level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}

};

struct db_building : db_entity, db_with_name {

	const Urho3D::IntVector2 size;
	const Urho3D::String icon;

	std::vector<db_cost*> costs;
	std::vector<db_building_level*> levels;

	std::vector<db_nation*> nations;

	db_building(short id, char* name, short sizeX, short sizeZ, char* icon)
		: db_entity(id), db_with_name(name),
		  size({sizeX, sizeZ}),
		  icon(icon) {
	}

	~db_building() {
		clear_vector(costs);
	}

	std::optional<db_building_level*> getLevel(short level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}
};

struct db_building_level : db_entity, db_level, db_with_name {
	const short building;
	const Urho3D::String nodeName;
	const short queueMaxCapacity;

	std::vector<db_cost*> costs;
	std::vector<db_unit*> allUnits;

	//std::vector<std::vector<db_unit*>> unitsPerNation;

	db_building_level(short id, short level, short building, char* name, char* nodeName, short queueMaxCapacity)
		: db_entity(id), db_level(level), db_with_name(name),
		  building(building),
		  nodeName(nodeName),
		  queueMaxCapacity(queueMaxCapacity) {
	}

	~db_building_level() {
		clear_vector(costs);
	}
};

struct db_hud_size : db_entity, db_with_name {

	db_hud_size(short id, char* name)
		: db_entity(id), db_with_name(name) {
	}
};

struct db_settings {
	short graph;
	short resolution;

	db_settings(short graph, short resolution)
		: graph(graph),
		  resolution(resolution) {
	}
};

struct db_resolution : db_entity {
	const short x;
	const short y;

	db_resolution(short id, short x, short y)
		: db_entity(id),
		  x(x),
		  y(y) {
	}
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
		  shadow(shadow) {
	}
};

struct db_nation : db_entity, db_with_name {
	std::vector<db_unit*> units;
	std::vector<db_building*> buildings;

	db_nation(short id, char* name)
		: db_entity(id), db_with_name(name) {
	}
};

struct db_resource : db_entity, db_with_name {
	const Urho3D::String icon;
	const short maxCapacity;
	Urho3D::Vector<Urho3D::String> nodeName;
	const Urho3D::IntVector2 size;
	const short maxUsers;
	const unsigned mini_map_color;

	db_resource(short id, char* name, char* icon, short maxCapacity, char* nodeName, short sizeX,
	            short sizeZ, short maxUsers, unsigned mini_map_color)
		: db_entity(id), db_with_name(name),
		  icon(icon),
		  maxCapacity(maxCapacity),
		  nodeName(Urho3D::String(nodeName).Split(SPLIT_SIGN)),
		  size(Urho3D::IntVector2(sizeX, sizeZ)),
		  maxUsers(maxUsers),
		  mini_map_color(mini_map_color) {
	}
};

struct db_hud_vars : db_entity, db_with_name {
	const short hud_size;
	float value;

	db_hud_vars(short id, short hudSize, char* name, float value)
		: db_entity(id), db_with_name(name),
		  hud_size(hudSize),
		  value(value) {
	}
};

struct db_cost {
	const short resource;
	const short value;

	db_cost(short resource, short value)
		: resource(resource),
		  value(value) {
	}
};

struct db_order : db_entity, db_with_name {
	const Urho3D::String icon;

	db_order(short id, char* icon, char* name)
		: db_entity(id), db_with_name(name),
		  icon(icon) {
	}
};

struct db_map : db_entity, db_with_name {
	const Urho3D::String height_map;
	const Urho3D::String texture;
	const float scale_hor;
	const float scale_ver;

	db_map(short id, char* heightMap, char* texture, float scaleHor, float scaleVer, char* name)
		: db_entity(id), db_with_name(name),
		  height_map(heightMap),
		  texture(texture),
		  scale_hor(scaleHor),
		  scale_ver(scaleVer) {
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
