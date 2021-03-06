﻿#pragma once
#include <magic_enum.hpp>
#include <span>
#include <vector>
#include <algorithm>
#include <functional>
#include <Urho3D/Math/Vector2.h>

#include "db_basic_struct.h"
#include "objects/Metrics.h"
#include "objects/unit/state/UnitState.h"
#include "simulation/SimGlobals.h"
#include "stats/AiWeights.h"
#include "utils/DeleteUtils.h"
#include "utils/OtherUtils.h"
#include "utils/StringUtils.h"

struct db_nation;
struct db_order;
struct db_building_level;
struct db_cost;
struct db_unit_level;

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
	const bool canBuildingAttack;


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
		  canBuildingAttack(initFlag(buildingAttackVal)) { }
};

struct db_static {
	const Urho3D::IntVector2 size;

	explicit db_static(const Urho3D::IntVector2& size)
		: size(size) { }
};

struct db_cost {
	const short resource;
	const short value;

	db_cost(short resource, short value)
		: resource(resource),
		  value(value) { }
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
protected:
	std::string paramsAString;
	std::span<float> paramsAsSpan;
	std::span<float> paramsNormAsSpan;
public:
	const std::span<float> getParamsAsSpan() const { return paramsAsSpan; }
	const std::span<float> getParamsNorm() const { return paramsNormAsSpan; }
	const std::string& getParamsNormAsString() const { return paramsAString; }
};

struct db_unit_metric : db_basic_metric {
private:
	float params[magic_enum::enum_count<UnitMetric>()];
	float paramsNorm[magic_enum::enum_count<UnitMetric>()];
public:

	db_unit_metric(float val1, float val2, float val3, float val4, float val5,
	               float val6)
		: params{val1, val2, val3, val4, val5, val6}, paramsNorm{val1, val2, val3, val4, val5, val6} {

		paramsAsSpan = std::span{params};
		paramsNormAsSpan = std::span{paramsNorm};

		assert(paramsNormAsSpan.size()==AI_WEIGHTS.wUnitInputSpan.size());

		std::transform(paramsNormAsSpan.begin(), paramsNormAsSpan.end(), AI_WEIGHTS.wUnitInputSpan.begin(),
		               paramsNormAsSpan.begin(), std::divides<>());

		paramsAString = join(paramsNormAsSpan);
	}
};

struct db_building_metric : db_basic_metric {
private:
	float params[magic_enum::enum_count<BuildingMetric>()];
	float paramsNorm[magic_enum::enum_count<BuildingMetric>()];
public:
	db_building_metric(float val1, float val2, float val3, float val4, float val5,
	                   float val6, float val7, float val8)
		: params{val1, val2, val3, val4, val5, val6, val7, val8},
		  paramsNorm{val1, val2, val3, val4, val5, val6, val7, val8} {
		paramsAsSpan = std::span{params};
		paramsNormAsSpan = std::span{paramsNorm};
		assert(paramsNormAsSpan.size()==AI_WEIGHTS.wBuildingInputSpan.size());
		std::transform(paramsNormAsSpan.begin(), paramsNormAsSpan.end(), AI_WEIGHTS.wBuildingInputSpan.begin(),
		               paramsNormAsSpan.begin(), std::divides<>());
		paramsAString = join(paramsNormAsSpan);
	}
};

struct db_level {
	const char level;

	explicit db_level(char level)
		: level(level) { }
};

struct db_with_hp {
	const unsigned short maxHp;
	const float invMaxHp;

	explicit db_with_hp(unsigned short maxHp)
		: maxHp(maxHp), invMaxHp(1.f / maxHp) { }
};

struct db_unit_level : db_entity, db_level, db_with_name, db_with_cost, db_attack, db_with_hp {
	const unsigned short unit;
	const float minDist;
	const float maxSep;
	const float mass;
	const float invMass;

	const float maxSpeed;
	const float minSpeed;
	const float collectSpeed;
	const float upgradeSpeed;
	const float maxForce;
	const float sqMinSpeed;
	const bool canCollect;

	const Urho3D::String nodeName;

	db_unit_metric* dbUnitMetric = nullptr;
	bool possibleStates[magic_enum::enum_count<UnitState>()];
	std::vector<unsigned char> ordersIds;

	db_unit_level(short id, short level, short unit, char* name, float minDist, float maxSep, char* nodeName,
	              float mass, short maxHp, float maxSpeed, float minSpeed, float upgradeSpeed, float collectSpeed,
	              float maxForce, float closeAttackVal, float rangeAttackVal, float chargeAttackVal,
	              float buildingAttackVal, short closeAttackSpeed, short rangeAttackSpeed, short rangeAttackRange,
	              float armor):
		db_entity(id), db_level(level), db_with_name(name),
		db_attack(closeAttackVal, rangeAttackVal, chargeAttackVal, buildingAttackVal,
		          closeAttackSpeed, rangeAttackSpeed, rangeAttackRange, armor),
		db_with_hp(maxHp),
		unit(unit),
		minDist(minDist),
		maxSep(maxSep),
		nodeName(nodeName),
		mass(mass),
		invMass(1 / mass),
		maxSpeed(maxSpeed),
		minSpeed(minSpeed),
		collectSpeed(collectSpeed),
		upgradeSpeed(upgradeSpeed),
		maxForce(maxForce),
		sqMinSpeed(minSpeed * minSpeed),
		canCollect(initFlag(collectSpeed)) { }

	void finish(float sumCreateCost) {
		dbUnitMetric = new db_unit_metric(armor * maxHp,
		                                  rangeAttackVal * rangeAttackSpeed,
		                                  closeAttackVal * closeAttackSpeed,
		                                  chargeAttackVal,
		                                  buildingAttackVal * rangeAttackSpeed,
		                                  sumCreateCost);
	}

	~db_unit_level() {
		delete dbUnitMetric;
	}
};

struct db_unit : db_entity, db_with_name, db_with_cost {
	const Urho3D::String icon;
	const short actionState;

	std::vector<db_unit_level*> levels;

	std::vector<db_nation*> nations;


	db_unit(short id, char* name, char* icon, short actionState)
		: db_entity(id), db_with_name(name),
		  icon(icon),
		  actionState(actionState) { }

	std::optional<db_unit_level*> getLevel(short level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}

	float getSumVal(UnitMetric unitMetric) const {
		float sum = 0.f;
		for (auto level : levels) {
			sum += level->dbUnitMetric->getParamsAsSpan()[cast(unitMetric)];
		}
		return sum;
	}
};

struct db_building : db_entity, db_with_name, db_with_cost, db_static {

	const Urho3D::String icon;

	std::vector<db_building_level*> levels;

	std::vector<db_nation*> nations;

	db_building(short id, char* name, short sizeX, short sizeZ, char* icon)
		: db_entity(id), db_with_name(name), db_static({sizeX, sizeZ}),
		  icon(icon) { }

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
	std::vector<std::vector<unsigned char>*> unitsPerNationIds; //TODO remember must be storted

	std::vector<db_building_metric*> dbBuildingMetricPerNation;

	db_building_level(short id, short level, short building, char* name, char* nodeName, short queueMaxCapacity,
	                  float rangeAttackVal, short rangeAttackSpeed, short rangeAttackRange, float armor, short maxHp)
		: db_entity(id), db_level(level), db_with_name(name),
		  db_attack(0.f, rangeAttackVal, 0.f, 0.f, 0.f, rangeAttackSpeed, rangeAttackRange, armor),
		  db_with_hp(maxHp),
		  building(building),
		  nodeName(nodeName),
		  queueMaxCapacity(queueMaxCapacity) { }

	~db_building_level() {
		clear_vector(unitsPerNation);
		clear_vector(unitsPerNationIds);
		clear_vector(dbBuildingMetricPerNation);
	}

	void finish(db_building* dbBuilding) {
		dbBuildingMetricPerNation.resize(unitsPerNation.size(), nullptr);
		for (int i = 0; i < unitsPerNation.size(); ++i) {
			auto dbUnits = unitsPerNation.at(i);
			if (dbUnits) {
				float sums[magic_enum::enum_count<UnitMetric>()];
				std::fill_n(sums, magic_enum::enum_count<UnitMetric>(), 0.f);
				for (auto dbUnit : *dbUnits) {
					constexpr auto& metrics = magic_enum::enum_values<UnitMetric>();
					for (int j = 0; j < magic_enum::enum_count<UnitMetric>(); ++j) {
						sums[cast(metrics[i])] += dbUnit->getSumVal(metrics[i]);
					}
				}
				dbBuildingMetricPerNation[i] =
					new db_building_metric(armor * maxHp,
					                       rangeAttackVal * rangeAttackSpeed,
					                       sums[cast(UnitMetric::DEFENCE)],
					                       sums[cast(UnitMetric::DISTANCE_ATTACK)],
					                       sums[cast(UnitMetric::CLOSE_ATTACK)],
					                       sums[cast(UnitMetric::CHARGE_ATTACK)],
					                       sums[cast(UnitMetric::BUILDING_ATTACK)],
					                       dbBuilding->getSumCost());
			}
		}
	}
};

struct db_nation : db_entity, db_with_name {
	std::vector<db_unit*> units;
	std::vector<db_building*> buildings;

	Urho3D::Vector<Urho3D::String> actionPrefix;
	Urho3D::Vector<Urho3D::String> orderPrefix;
	Urho3D::Vector<Urho3D::String> orderThresholdPrefix;

	db_nation(short id, char* name, char* actionPrefix, char* orderPrefix, char* orderThresholdPrefix)
		: db_entity(id), db_with_name(name),
		  actionPrefix(Urho3D::String(actionPrefix).Split(SPLIT_SIGN)),
		  orderPrefix(Urho3D::String(orderPrefix).Split(SPLIT_SIGN)),
		  orderThresholdPrefix(Urho3D::String(orderThresholdPrefix).Split(SPLIT_SIGN)) { }

	void refresh() {
		assert(id<MAX_PLAYERS); //TODO BUG to sa troszke inne rzeczy
		if (!SimGlobals::ACTION_AI_PATH[id].Empty()) {
			this->actionPrefix = SimGlobals::ACTION_AI_PATH[id].Split(SPLIT_SIGN);
		}
		if (!SimGlobals::ORDER_AI_PATH[id].Empty()) {
			this->orderPrefix = SimGlobals::ORDER_AI_PATH[id].Split(SPLIT_SIGN);
		}
		if (!SimGlobals::ORDER_THRESHOLD_PATH[id].Empty()) {
			this->orderThresholdPrefix = SimGlobals::ORDER_THRESHOLD_PATH[id].Split(SPLIT_SIGN);
		}
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
		  mini_map_color(mini_map_color) { }
};
