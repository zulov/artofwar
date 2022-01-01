#pragma once
#include <magic_enum.hpp>
#include <span>
#include <vector>
#include <algorithm>
#include <functional>
#include <Urho3D/Math/Vector2.h>

#include "db_basic_struct.h"
#include "math/SpanUtils.h"
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

#define UNIT_METRIC_NUMBER 10
#define BUILDING_METRIC_NUMBER 10

float inline safeDiv(float first, short second) {
	second = second <= 0 ? 1 : second;
	return first > 0.f ? first / second * FRAMES_IN_PERIOD : 0.f;
}

struct db_attack {
	const float collect;

	const float attackMelee;
	const short attackMeleeReload;

	const float attackRange;
	const float attackRangeReload;
	const short attackRangeRange;
	const short sqAttackRangeRange;

	const float attackCharge;

	const bool typeInfantry;
	const bool typeRange;
	const bool typeCalvary;
	const bool typeWorker;
	const bool typeSpecial;
	const bool typeMelee;
	const bool typeHeavy;
	const bool typeLight;

	const float bonusInfantry;
	const float bonusRange;
	const float bonusCalvary;
	const float bonusWorker;
	const float bonusSpecial;
	const float bonusMelee;
	const float bonusHeavy;
	const float bonusLight;
	const float bonusBuilding;

	bool initFlag(float val) {
		if (val > 0.f) {
			return true;
		}
		return false;
	}

	db_attack(float collect, float attackMelee, short attackMeleeReload, float attackRange, float attackRangeReload,
	          short attackRangeRange, float attackCharge, bool typeInfantry, bool typeRange,
	          bool typeCalvary, bool typeWorker, bool typeSpecial, bool typeMelee, bool typeHeavy, bool typeLight,
	          float bonusInfantry, float bonusRange, float bonusCalvary, float bonusWorker, float bonusSpecial,
	          float bonusMelee, float bonusHeavy, float bonusLight, float bonusBuilding)
		: collect(collect),
		  attackMelee(attackMelee),
		  attackMeleeReload(attackMeleeReload),
		  attackRange(attackRange),
		  attackRangeReload(attackRangeReload),
		  attackRangeRange(attackRangeRange),
		  sqAttackRangeRange(attackRangeRange * attackRangeRange),
		  attackCharge(attackCharge),
		  typeInfantry(typeInfantry),
		  typeRange(typeRange),
		  typeCalvary(typeCalvary),
		  typeWorker(typeWorker),
		  typeSpecial(typeSpecial),
		  typeMelee(typeMelee),
		  typeHeavy(typeHeavy),
		  typeLight(typeLight),
		  bonusInfantry(bonusInfantry),
		  bonusRange(bonusRange),
		  bonusCalvary(bonusCalvary),
		  bonusWorker(bonusWorker),
		  bonusSpecial(bonusSpecial),
		  bonusMelee(bonusMelee),
		  bonusHeavy(bonusHeavy),
		  bonusLight(bonusLight),
		  bonusBuilding(bonusBuilding) {
	}
};

struct db_static {
	const Urho3D::IntVector2 size;

	explicit db_static(const Urho3D::IntVector2& size)
		: size(size) {
	}
};

struct db_cost {
	const std::array<short, 4> values;

	const short sum = 0;

	db_cost(short food, short wood, short stone, short gold) : values({food, wood, stone, gold}),
	                                                           sum(food + wood + stone + gold) {
	}
};

struct db_with_cost {
	db_cost* costs{};

	unsigned short getSumCost() const {
		return costs ? costs->sum : 0;
	}

	~db_with_cost() {
		delete costs;
	}
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
	float params[UNIT_METRIC_NUMBER];
	float paramsNorm[UNIT_METRIC_NUMBER];
public:
	db_unit_metric(float val1, float val2, float val3, float val4, float val5,
	               float val6)
		: params{val1, val2, val3, val4, val5, val6}, paramsNorm{val1, val2, val3, val4, val5, val6} {

		paramsAsSpan = std::span{params};
		paramsNormAsSpan = std::span{paramsNorm};
		assert(validateSpan(__LINE__, __FILE__, params));
		assert(paramsNormAsSpan.size()==AI_WEIGHTS.wUnitInputSpan.size());

		std::transform(paramsNormAsSpan.begin(), paramsNormAsSpan.end(), AI_WEIGHTS.wUnitInputSpan.begin(),
		               paramsNormAsSpan.begin(), std::divides<>());

		paramsAString = join(paramsNormAsSpan);
	}
};

struct db_building_metric : db_basic_metric {
private:
	float params[BUILDING_METRIC_NUMBER];
	float paramsNorm[BUILDING_METRIC_NUMBER];
public:
	db_building_metric(float val1, float val2, float val3, float val4, float val5,
	                   float val6, float val7, float val8)
		: params{val1, val2, val3, val4, val5, val6, val7, val8},
		  paramsNorm{val1, val2, val3, val4, val5, val6, val7, val8} {
		paramsAsSpan = std::span{params};
		paramsNormAsSpan = std::span{paramsNorm};
		assert(validateSpan(__LINE__, __FILE__, params));
		assert(paramsNormAsSpan.size()==AI_WEIGHTS.wBuildingInputSpan.size());
		std::transform(paramsNormAsSpan.begin(), paramsNormAsSpan.end(), AI_WEIGHTS.wBuildingInputSpan.begin(),
		               paramsNormAsSpan.begin(), std::divides<>());
		paramsAString = join(paramsNormAsSpan);
	}
};

struct db_level {
	const char level;

	explicit db_level(char level)
		: level(level) {
	}
};

struct db_sight {
	const float sightRadius;

	explicit db_sight(float sightRadius)
		: sightRadius(sightRadius) {
	}
};

struct db_with_hp {
	const unsigned short maxHp;
	const float invMaxHp;
	const float armor;

	explicit db_with_hp(unsigned short maxHp, float armor)
		: maxHp(maxHp), invMaxHp(1.f / maxHp), armor(armor) {
	}
};

struct db_with_model {
	float modelHeight = -1.f;
};

struct db_build_upgrade {
	short buildTime = -1;
	short upgradeTime = -1;

	db_build_upgrade(short buildTime, short upgradeTime)
		: buildTime(buildTime), upgradeTime(upgradeTime) {
	}
};

struct db_unit_level : db_entity, db_level, db_with_name, db_with_cost, db_attack, db_with_hp, db_sight, db_with_model,
                       db_build_upgrade {
	const bool canCollect;
	const unsigned short unit;

	const float minDist;
	const float maxSep;
	const float mass;
	const float invMass;

	const float maxSpeed;
	const float minSpeed;

	const float maxForce;
	const float sqMinSpeed;

	const Urho3D::String node;

	db_unit_metric* dbUnitMetric = nullptr;
	bool possibleStates[magic_enum::enum_count<UnitState>()];
	std::vector<unsigned char> ordersIds;

	db_unit_level(short id, short level, short unit, char* name, char* node, short buildTime, short upgradeTime,
	              float minDist, float mass, float minSpeed, float maxSpeed, int maxForce, float sightRng,
	              short maxHp, float armor, float collect, float atckM, short atckMRld, float atckR,
	              float atckRRld, short atckRRng, float atckCH, bool tI, bool tR, bool tC, bool tW, bool tS, bool tM,
	              bool tH, bool tL,
	              float bI, float bR, float bC, float bW, float bS, float bM, float bH, float bL, float bB):
		db_entity(id), db_level(level), db_with_name(name),
		db_attack(),
		db_with_hp(maxHp, armor), db_sight(sightRng), db_build_upgrade(buildTime, upgradeTime),
		unit(unit),
		minDist(minDist),
		maxSep(maxSep),
		node(node),
		mass(mass),
		invMass(1 / mass),
		maxSpeed(maxSpeed),
		minSpeed(minSpeed),
		maxForce(maxForce),
		sqMinSpeed(minSpeed * minSpeed),
		canCollect(initFlag(collect)) {
	}

	void finish(float sumCreateCost) {
		dbUnitMetric = new db_unit_metric(armor * maxHp,
		                                  safeDiv(rangeAttackVal, rangeAttackReload),
		                                  safeDiv(closeAttackVal, closeAttackReload),
		                                  chargeAttackVal,
		                                  safeDiv(buildingAttackVal, closeAttackReload), //TODO jaki reload?
		                                  sumCreateCost);
	}

	~db_unit_level() {
		delete dbUnitMetric;
	}
};

struct db_unit : db_with_name, db_with_cost, db_entity {
	const UnitState actionState;
	const Urho3D::String icon;

	std::vector<db_unit_level*> levels;

	std::vector<db_nation*> nations;

	db_unit(short id, char* name, char* icon, short actionState)
		: db_entity(id), db_with_name(name),
		  icon(icon),
		  actionState(UnitState(actionState)) {
	}

	std::optional<db_unit_level*> getLevel(short level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
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

struct db_building_level : db_with_name, db_with_cost, db_entity, db_level, db_sight, db_with_hp, db_attack,
                           db_with_model, db_build_upgrade {
	const short building;
	const short queueMaxCapacity;
	const Urho3D::String nodeName;

	std::vector<db_unit*> allUnits;

	std::vector<std::vector<db_unit*>*> unitsPerNation;
	std::vector<std::vector<unsigned char>*> unitsPerNationIds; //TODO remember must be storted

	std::vector<db_building_metric*> dbBuildingMetricPerNation;

	db_building_level(short id, short level, short building, char* name, char* nodeName, short queueMaxCapacity,
	                  float rangeAttackVal, short rangeAttackSpeed, float rangeAttackRange, float armor, short maxHp,
	                  short buildSpeed, short upgradeSpeed)
		: db_entity(id), db_level(level), db_with_name(name),
		  db_attack(0.f, rangeAttackVal, 0.f, 0.f, 0.f, rangeAttackSpeed, rangeAttackRange, armor, 0.f, 0.f),
		  db_with_hp(maxHp), db_sight(15.f), db_build_upgrade(buildSpeed, upgradeSpeed),
		  building(building), nodeName(nodeName), queueMaxCapacity(queueMaxCapacity) {
	}

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
				
			}
		}
	}
};

struct db_nation : db_entity, db_with_name {
	std::vector<db_unit*> units;
	std::vector<db_building*> buildings;

	std::vector<std::string> actionPrefix;
	std::vector<std::string> orderPrefix;
	std::vector<std::string> orderThresholdPrefix;

	db_nation(short id, char* name, char* actionPrefix, char* orderPrefix, char* orderThresholdPrefix)
		: db_entity(id), db_with_name(name),
		  actionPrefix(split(actionPrefix, SPLIT_SIGN)),
		  orderPrefix(split(orderPrefix, SPLIT_SIGN)),
		  orderThresholdPrefix(split(orderThresholdPrefix, SPLIT_SIGN)) {
	}

	void refresh() {
		assert(id<MAX_PLAYERS); //TODO BUG to sa troszke inne rzeczy
		if (!SimGlobals::ACTION_AI_PATH[id].empty()) {
			this->actionPrefix = split(SimGlobals::ACTION_AI_PATH[id], SPLIT_SIGN);
		}
		if (!SimGlobals::ORDER_AI_PATH[id].empty()) {
			this->orderPrefix = split(SimGlobals::ORDER_AI_PATH[id], SPLIT_SIGN);
		}
		if (!SimGlobals::ORDER_THRESHOLD_PATH[id].empty()) {
			this->orderThresholdPrefix = split(SimGlobals::ORDER_THRESHOLD_PATH[id], SPLIT_SIGN);
		}
	}
};

struct db_resource : db_with_name, db_static, db_with_hp, db_entity, db_with_model {
	const short maxUsers;
	const unsigned mini_map_color;
	const Urho3D::String icon;
	Urho3D::Vector<Urho3D::String> nodeName;


	db_resource(short id, char* name, char* icon, unsigned short maxHp, char* nodeName, short sizeX,
	            short sizeZ, short maxUsers, unsigned mini_map_color)
		: db_entity(id), db_with_name(name), db_static({sizeX, sizeZ}), db_with_hp(maxHp),
		  icon(icon),
		  nodeName(Urho3D::String(nodeName).Split(SPLIT_SIGN)),
		  maxUsers(maxUsers),
		  mini_map_color(mini_map_color) {
	}
};
