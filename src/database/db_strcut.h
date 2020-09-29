#pragma once

constexpr char SPLIT_SIGN = '\n';
constexpr char AI_PROPS_SIZE = 4;

#include <magic_enum.hpp>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Math/Vector2.h>
#include "objects/unit/state/UnitState.h"
#include "utils/DeleteUtils.h"

struct db_nation;
struct db_order;
struct db_building_level;
struct db_cost;
struct db_unit_level;

struct db_entity {
	const short id;

	explicit db_entity(short id)
		: id(id) {
	}
};

struct db_attack {
	const float closeAttackVal;
	const float rangeAttackVal;
	const float chargeAttackVal;
	const float buildingAttackVal;

	const float armor;

	const short closeAttackSpeed;
	const short rangeAttackSpeed;

	const short closeAttackRange;
	const short rangeAttackRange;
	const short sqCloseAttackRange;
	const short sqRangeAttackRange;

	const float attackInterest;
	const float sqAttackInterest;

	const bool canCloseAttack;
	const bool canRangeAttack;
	const bool canChargeAttack;
	const bool canCollectAttack;

	bool initFlag(float val) {
		if (val > 0.f) {
			return true;
		}
		return false;
	}

	db_attack(float closeAttackVal, float rangeAttackVal, float chargeAttackVal, float buildingAttackVal,
	          short closeAttackSpeed, short rangeAttackSpeed, short rangeAttackRange, float armor)
		: closeAttackVal(closeAttackVal),
		  rangeAttackVal(rangeAttackVal),
		  chargeAttackVal(chargeAttackVal),
		  buildingAttackVal(buildingAttackVal),
	  armor(armor),
		  closeAttackSpeed(closeAttackSpeed),
		  rangeAttackSpeed(rangeAttackSpeed),
		  rangeAttackRange(rangeAttackRange), sqRangeAttackRange(rangeAttackRange * rangeAttackRange),
		  closeAttackRange(1.f), sqCloseAttackRange(1.f),
		  attackInterest(rangeAttackRange * 10.f),
		  sqAttackInterest(attackInterest * attackInterest),
		  canCloseAttack(initFlag(closeAttackVal)),
		  canRangeAttack(initFlag(rangeAttackVal)),
		  canChargeAttack(initFlag(chargeAttackVal)),
		  canCollectAttack(initFlag(buildingAttackVal)) {
	}
};

struct db_with_name {
	const Urho3D::String name;

	explicit db_with_name(const Urho3D::String& name)
		: name(name) {
	}
};

struct db_static {
	const Urho3D::IntVector2 size;

	explicit db_static(const Urho3D::IntVector2& size)
		: size(size) {
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

struct db_with_cost {
	std::vector<db_cost*> costs;

	unsigned short getSumCost() {
		unsigned short sum = 0;
		for (auto cost : costs) {
			sum += cost->value;
		}
		return sum;
	}

	~db_with_cost() { clear_vector(costs); }
};

struct db_basic_metric {
private:
	float costSum = 0;

	std::string paramsAString;
	std::span<float> paramsAsSpan;
public:
	float params[AI_PROPS_SIZE];

	const float defence;
	const float distanceAttack;


	db_basic_metric(float costSum, float defence, float distanceAttack)
		: costSum(costSum), defence(defence),
		  distanceAttack(distanceAttack), params{costSum, defence, distanceAttack} {
	}
};

struct db_unit_metric : db_basic_metric {
private:
	const float closeAttack;
	const float chargeAttack;
public:

	db_unit_metric(float costSum, float defence, float distanceAttack, float closeAttack, float chargeAttack)
		: db_basic_metric(costSum, defence, distanceAttack),
		  closeAttack(closeAttack),
		  chargeAttack(chargeAttack) {
	}
};

struct db_building_metric : db_basic_metric {
public:

	db_building_metric(float costSum, float defence, float distanceAttack)
		: db_basic_metric(costSum, defence, distanceAttack) {
	}
};

struct db_ai_property {
private:
	float costSum = 0;

	std::string paramsAString;
	std::span<float> paramsAsSpan;
public:
	float params[AI_PROPS_SIZE];

	const float econ;
	const float attack;
	const float defence;

	db_ai_property(float econ, float attack, float defence)
		: econ(econ),
		  attack(attack),
		  defence(defence), params{econ, attack, defence} {
	}

	const std::string& getParamsNormAsString() const { return paramsAString; }
	const std::span<float> getParamsNormAsSpan() const { return paramsAsSpan; }

	void setCostSum(float costSum, float normDiv) {
		this->costSum = costSum;
		params[3] = costSum / normDiv;
	}

	void setParamsNormAString(std::string paramsAString) {
		this->paramsAString = std::move(paramsAString);
		paramsAsSpan = std::span{params};
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

struct db_with_hp {
	const unsigned short maxHp;

	explicit db_with_hp(unsigned short maxHp)
		: maxHp(maxHp) {
	}
};

struct db_unit : db_entity, db_with_name, db_with_cost {
	const bool rotatable;
	const Urho3D::String icon;
	const short actionState;

	std::vector<db_unit_level*> levels;

	std::vector<db_order*> orders;
	std::vector<short> ordersIds;
	std::vector<db_nation*> nations;

	bool possibleStates[magic_enum::enum_count<UnitState>()];

	db_unit(short id, char* name, short rotatable, char* icon, short actionState)
		: db_entity(id), db_with_name(name),
		  rotatable(rotatable),
		  icon(icon),
		  actionState(actionState) {
	}

	std::optional<db_unit_level*> getLevel(short level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}

};

struct db_unit_level : db_entity, db_level, db_with_name, db_with_cost, db_attack, db_with_hp {
	const unsigned short unit;
	const float minDist;
	const float maxSep;
	const float mass;

	const float maxSpeed;
	const float minSpeed;
	const float collectSpeed;
	const float upgradeSpeed;
	const float maxForce;
	const float sqMinSpeed;

	const Urho3D::String nodeName;

	db_unit_metric* dbUnitMetric = nullptr;

	db_unit_level(short id, short level, short unit, char* name, float minDist, float maxSep, char* nodeName,
	              float mass, short maxHp, float maxSpeed, float minSpeed, float upgradeSpeed,
	              float collectSpeed, float maxForce, float closeAttackVal, float rangeAttackVal, float chargeAttackVal,
	              float buildingAttackVal, short closeAttackSpeed, short rangeAttackSpeed, short rangeAttackRange,
	              float armor): db_entity(id), db_level(level), db_with_name(name),
	                            db_attack(closeAttackVal, rangeAttackVal, chargeAttackVal, buildingAttackVal,
	                                      closeAttackSpeed, rangeAttackSpeed, rangeAttackRange, armor),
	                            db_with_hp(maxHp),
	                            unit(unit),
	                            minDist(minDist),
	                            maxSep(maxSep),
	                            nodeName(nodeName),
	                            mass(mass),
	                            maxSpeed(maxSpeed),
	                            minSpeed(minSpeed),
	                            collectSpeed(collectSpeed),
	                            upgradeSpeed(upgradeSpeed),
	                            maxForce(maxForce),
	                            sqMinSpeed(minSpeed * minSpeed) {
	}

	void finish(db_unit* dbUnit) {

		dbUnitMetric = new db_unit_metric(dbUnit->getSumCost(), armor * maxHp,
		                                  rangeAttackVal * rangeAttackSpeed,
		                                  closeAttackVal * closeAttackSpeed,
		                                  chargeAttackVal);
	}

	~db_unit_level() {
		delete dbUnitMetric;
	}
};

struct db_building : db_entity, db_with_name, db_with_cost, db_static {

	const Urho3D::String icon;

	std::vector<db_building_level*> levels;

	std::vector<db_nation*> nations;

	db_building(short id, char* name, short sizeX, short sizeZ, char* icon)
		: db_entity(id), db_with_name(name), db_static({sizeX, sizeZ}),
		  icon(icon) {
	}

	std::optional<db_building_level*> getLevel(short level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}
};

struct db_building_level : db_entity, db_level, db_with_name, db_with_cost, db_attack, db_with_hp {
	const short building;
	const Urho3D::String nodeName;
	const short queueMaxCapacity;

	std::vector<db_unit*> allUnits;

	std::vector<std::vector<db_unit*>*> unitsPerNation;
	std::vector<std::vector<short>*> unitsPerNationIds; //TODO remember must be storted

	db_building_metric* dbBuildingMetric = nullptr;

	db_building_level(short id, short level, short building, char* name, char* nodeName, short queueMaxCapacity,
	                  float rangeAttackVal, short rangeAttackSpeed, short rangeAttackRange, float armor)
		: db_entity(id), db_level(level), db_with_name(name),
		  db_attack(0.f, rangeAttackVal, 0.f, 0.f, 0.f, rangeAttackSpeed, rangeAttackRange, armor),
		  db_with_hp(maxHp),
		  building(building),
		  nodeName(nodeName),
		  queueMaxCapacity(queueMaxCapacity) {
	}

	~db_building_level() {
		clear_vector(unitsPerNation);
		clear_vector(unitsPerNationIds);
		delete dbBuildingMetric;
	}

	void finish(db_building* dbBuilding) {
		dbBuildingMetric = new db_building_metric(dbBuilding->getSumCost(), armor * maxHp,
		                                          rangeAttackVal * rangeAttackSpeed);
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

struct db_resource : db_entity, db_with_name, db_static, db_with_hp {
	const Urho3D::String icon;
	Urho3D::Vector<Urho3D::String> nodeName;
	const short maxUsers;
	const unsigned mini_map_color;

	db_resource(short id, char* name, char* icon, unsigned short maxHp, char* nodeName, short sizeX,
	            short sizeZ, short maxUsers, unsigned mini_map_color)
		: db_entity(id), db_with_name(name), db_static({sizeX, sizeZ}), db_with_hp(maxHp),
		  icon(icon),
		  nodeName(Urho3D::String(nodeName).Split(SPLIT_SIGN)),
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

	void finish() {
		for (auto* unitLevel : unitsLevels) {
			unitLevel->finish(units[unitLevel->unit]);
		}

		for (auto* buildingLevel : buildingsLevels) {
			buildingLevel->finish(buildings[buildingLevel->building]);
		}
	}
};
