#pragma once
#include <magic_enum.hpp>
#include <span>
#include <vector>
#include <algorithm>
#include <functional>
#include <valarray>
#include <Urho3D/Math/Vector2.h>

#include "db_basic_struct.h"
#include "math/SpanUtils.h"
#include "objects/building/BuildingType.h"
#include "objects/unit/state/UnitState.h"
#include "simulation/SimGlobals.h"
#include "utils/DeleteUtils.h"
#include "utils/OtherUtils.h"
#include "utils/StringUtils.h"

struct db_unit;
struct db_unit_metric;
struct db_nation;
struct db_order;
struct db_building_level;
struct db_cost;
struct db_unit_level;

float inline safeDiv(float first, short second) {
	second = second <= 0 ? 1 : second;
	return first > 0.f ? first / second * FRAMES_IN_PERIOD : 0.f;
}

struct db_common_attack {
	const float collect;
	const float attack;
	const float attackReload;
	const short attackRange;
	const short sqAttackRange;

	db_common_attack(float collect, float attack, float attackReload, short attackRange)
		: attack(attack), attackReload(attackReload), attackRange(attackRange), collect(collect),
		  sqAttackRange(attackRange * attackRange) { }

	bool initFlag(float val) const {
		return val > 0.f;
	}
};


struct db_building_attack : db_common_attack {
	const bool canAttack;

	db_building_attack(float collect, float attack, float attackReload, short attackRange)
		: db_common_attack(collect, attack, attackReload, attackRange),
		  canAttack(initFlag(attack)) { }
};

struct db_unit_attack : db_common_attack {
	const float bonusInfantry;
	const float bonusRange;
	const float bonusCalvary;
	const float bonusWorker;
	const float bonusSpecial;
	const float bonusMelee;
	const float bonusHeavy;
	const float bonusLight;
	const float bonusBuilding;

	db_unit_attack(float collect, float attack, float attackReload, short attackRange,
	               float bonusInfantry, float bonusRange, float bonusCalvary, float bonusWorker,
	               float bonusSpecial, float bonusMelee, float bonusHeavy, float bonusLight, float bonusBuilding)
		: db_common_attack(collect, attack, attackReload, attackRange),
		  bonusInfantry(bonusInfantry),
		  bonusRange(bonusRange),
		  bonusCalvary(bonusCalvary),
		  bonusWorker(bonusWorker),
		  bonusSpecial(bonusSpecial),
		  bonusMelee(bonusMelee),
		  bonusHeavy(bonusHeavy),
		  bonusLight(bonusLight),
		  bonusBuilding(bonusBuilding) { }
};

struct db_static {
	const Urho3D::IntVector2 size;

	explicit db_static(const Urho3D::IntVector2& size)
		: size(size) { }
};

struct db_cost {
	const std::array<short, 4> values;

	const short sum = 0;

	db_cost(short food, short wood, short stone, short gold) : values({food, wood, stone, gold}),
	                                                           sum(food + wood + stone + gold) { }
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

	std::vector<float> valuesNorm;
	std::valarray<float> valuesNormAsVal;

	std::vector<float> valuesNormForSum;
public:
	const std::vector<float>& getValuesNorm() const { return valuesNorm; }
	const std::valarray<float>& getValuesNormAsVal() const { return valuesNormAsVal; }
	const std::vector<float>& getValuesNormForSum() const { return valuesNormForSum; }
	const std::string& getParamsNormAsString() const { return paramsAString; }
};

struct db_building_metric : db_basic_metric {
	//std::vector<float> valuesOtherNorm;
	//std::vector<float> valuesDefenceNorm;
	//std::vector<float> valuesResourceNorm;
	//std::vector<float> valuesTechNorm;
	//std::vector<float> valuesUnitsNorm;

	std::valarray<float> valuesOtherNormAsVal;
	std::valarray<float> valuesDefenceNormAsVal;
	std::valarray<float> valuesResourceNormAsVal;
	std::valarray<float> valuesTechNormAsVal;
	std::valarray<float> valuesUnitsNormAsVal;

	db_building_metric(const std::vector<float>& newValues, const std::vector<float>& newValuesForSum,
	                   std::vector<float>& valuesOtherNorm, std::vector<float>& valuesDefenceNorm,
	                   std::vector<float>& valuesResourceNorm, std::vector<float>& valuesTechNorm,
	                   std::vector<float>& valuesUnitsNorm) {
		valuesNorm.insert(valuesNorm.end(), newValues.begin(), newValues.end());
		valuesNormForSum.insert(valuesNormForSum.end(), newValuesForSum.begin(), newValuesForSum.end());


		valuesNormAsVal = std::valarray(*valuesNorm.data(), valuesNorm.size());
		valuesOtherNormAsVal = std::valarray(*valuesOtherNorm.data(), valuesOtherNorm.size());
		valuesDefenceNormAsVal = std::valarray(*valuesDefenceNorm.data(), valuesDefenceNorm.size());
		valuesResourceNormAsVal = std::valarray(*valuesResourceNorm.data(), valuesResourceNorm.size());
		valuesTechNormAsVal = std::valarray(*valuesTechNorm.data(), valuesTechNorm.size());
		valuesUnitsNormAsVal = std::valarray(*valuesUnitsNorm.data(), valuesUnitsNorm.size());

		assert(validateSpan(__LINE__, __FILE__, valuesNorm));
		assert(validateSpan(__LINE__, __FILE__, valuesNormForSum));
		//assert(validateSpan(__LINE__, __FILE__, valuesNormSmall));

		paramsAString = join(valuesNorm);
	}

	const std::valarray<float>& getValuesNormAsValForType(ParentBuildingType type) const {
		switch (type) {
		case ParentBuildingType::OTHER:
			return valuesOtherNormAsVal;
		case ParentBuildingType::DEFENCE:
			return valuesDefenceNormAsVal;
		case ParentBuildingType::RESOURCE:
			return valuesResourceNormAsVal;
		case ParentBuildingType::TECHNOLOGY:
			return valuesTechNormAsVal;
		case ParentBuildingType::UNITS:
			return valuesUnitsNormAsVal;
		default: ;
		}
	};
};

struct db_level {
	const char level;

	explicit db_level(char level)
		: level(level) { }
};

struct db_with_hp {
	const unsigned short maxHp;
	const float invMaxHp;
	const float armor;

	explicit db_with_hp(unsigned short maxHp, float armor)
		: maxHp(maxHp), invMaxHp(1.f / maxHp), armor(armor) { }
};

struct db_base : db_with_hp {
	const float sightRadius;
	const float sqSightRadius;

	const float interestRange;
	const float sqInterestRange;

	db_base(unsigned short maxHp, float armor, float sightRadius)
		: db_with_hp(maxHp, armor), sightRadius(sightRadius), sqSightRadius(sightRadius * sightRadius),
		  interestRange(sightRadius * 0.8f), sqInterestRange(interestRange * interestRange) { }

};

struct db_with_model {
	float modelHeight = -1.f;
};

struct db_build_upgrade {
	short buildTime = -1;
	short upgradeTime = -1;

	db_build_upgrade(short buildTime, short upgradeTime)
		: buildTime(buildTime), upgradeTime(upgradeTime) { }
};

struct db_unit_metric : db_basic_metric {

	std::vector<float> valuesNormSmall;

	const std::vector<float>& getValuesNormSmall() const { return valuesNormSmall; }

	db_unit_metric(const std::vector<float>& newValues, const std::vector<float>& newValuesForSum,
	               const std::vector<float>& newValuesSmall) {
		valuesNorm.insert(valuesNorm.end(), newValues.begin(), newValues.end());
		valuesNormForSum.insert(valuesNormForSum.end(), newValuesForSum.begin(), newValuesForSum.end());
		valuesNormSmall.insert(valuesNormSmall.end(), newValuesSmall.begin(), newValuesSmall.end());

		valuesNormAsVal = std::valarray(*valuesNorm.data(), valuesNorm.size());

		assert(validateSpan(__LINE__, __FILE__, valuesNorm));
		assert(validateSpan(__LINE__, __FILE__, valuesNormForSum));
		assert(validateSpan(__LINE__, __FILE__, valuesNormSmall));

		paramsAString = join(valuesNorm);
	}
};

struct db_unit_level : db_entity, db_level, db_with_name, db_with_cost, db_unit_attack, db_base, db_with_model,
                       db_build_upgrade {
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

	db_unit_metric* dbUnitMetric = nullptr; //TODO jak to zrobic

	db_unit_level(short id, short level, short unit, char* name, char* node, short buildTime, short upgradeTime,
	              float minDist, float mass, float minSpeed, float maxSpeed, int maxForce,
	              short maxHp, float armor, float sightRng, float collect, float atck, float atckRld, short atckRng,
	              float bI, float bR, float bC, float bW, float bS, float bM, float bH, float bL, float bB):
		db_entity(id), db_level(level), db_with_name(name),
		db_unit_attack(collect, atck, atckRld, atckRng, bI, bR, bC, bW, bS, bM, bH, bL, bB),
		db_base(maxHp, armor, sightRng), db_build_upgrade(buildTime, upgradeTime),
		unit(unit),
		minDist(minDist),
		maxSep(1.f),
		node(node),
		mass(mass),
		invMass(1 / mass),
		maxSpeed(maxSpeed),
		minSpeed(minSpeed),
		maxForce(maxForce),
		sqMinSpeed(minSpeed * minSpeed) { }

	void finish(const std::vector<float>& newValues, const std::vector<float>& newValuesForSum,
	            const std::vector<float>& newValuesSmall) {
		dbUnitMetric = new db_unit_metric(newValues, newValuesForSum, newValuesSmall);
	}

	~db_unit_level() {
		delete dbUnitMetric;
	}

};

struct db_unit : db_with_name, db_with_cost, db_entity {
	const UnitState actionState;
	const Urho3D::String icon;

	const bool typeInfantry;
	const bool typeRange;
	const bool typeCalvary;
	const bool typeWorker;
	const bool typeSpecial;
	const bool typeMelee;
	const bool typeHeavy;
	const bool typeLight;

	std::vector<db_unit_level*> levels;

	std::vector<db_nation*> nations;
	std::vector<unsigned char> ordersIds;
	bool possibleStates[magic_enum::enum_count<UnitState>()];

	db_unit(short id, char* name, char* icon, char actionState, bool typeInfantry, bool typeRange,
	        bool typeCalvary, bool typeWorker, bool typeSpecial, bool typeMelee, bool typeHeavy, bool typeLight) :
		db_with_name(name), db_entity(id),
		actionState(UnitState(actionState)),
		icon(icon),
		typeInfantry(typeInfantry),
		typeRange(typeRange),
		typeCalvary(typeCalvary),
		typeWorker(typeWorker),
		typeSpecial(typeSpecial),
		typeMelee(typeMelee),
		typeHeavy(typeHeavy),
		typeLight(typeLight) { }

	std::optional<db_unit_level*> getLevel(short level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}

	float getBonuses(db_unit_attack* dbLevel) const {
		return typeInfantry * dbLevel->bonusInfantry +
			typeRange * dbLevel->bonusRange +
			typeCalvary * dbLevel->bonusCalvary +
			typeWorker * dbLevel->bonusWorker +
			typeSpecial * dbLevel->bonusSpecial +
			typeMelee * dbLevel->bonusMelee +
			typeHeavy * dbLevel->bonusHeavy +
			typeLight * dbLevel->bonusLight;
	}
};


struct db_building : db_entity, db_with_name, db_with_cost, db_static {
	const Urho3D::String icon;
	bool typeCenter;
	bool typeHome;
	bool typeDefence;
	bool typeResourceFood;
	bool typeResourceWood;
	bool typeResourceStone;
	bool typeResourceGold;
	bool typeTechBlacksmith;
	bool typeTechUniversity;
	bool typeUnitBarracks;
	bool typeUnitRange;
	bool typeUnitCavalry;

	bool typeResourceAny;
	std::vector<char> resourceTypes;
	bool hasResourceType[RESOURCES_SIZE];
	bool parentType[magic_enum::enum_count<ParentBuildingType>()];


	std::vector<db_building_level*> levels;

	std::vector<db_nation*> nations;


	db_building(short id, char* name, char* icon, short sizeX, short sizeZ,
	            bool typeCenter, bool typeHome, bool typeDefence, bool typeResourceFood, bool typeResourceWood,
	            bool typeResourceStone, bool typeResourceGold, bool typeTechBlacksmith, bool typeTechUniversity,
	            bool typeUnitBarracks, bool typeUnitRange, bool typeUnitCavalry)
		: db_entity(id), db_with_name(name), db_static({sizeX, sizeZ}), icon(icon),
		  typeCenter(typeCenter), typeHome(typeHome), typeDefence(typeDefence),
		  typeResourceFood(typeResourceFood), typeResourceWood(typeResourceWood),
		  typeResourceStone(typeResourceStone), typeResourceGold(typeResourceGold),
		  typeTechBlacksmith(typeTechBlacksmith), typeTechUniversity(typeTechUniversity),
		  typeUnitBarracks(typeUnitBarracks), typeUnitRange(typeUnitRange), typeUnitCavalry(typeUnitCavalry),
		  typeResourceAny(typeResourceFood || typeResourceWood || typeResourceStone || typeResourceGold) {
		hasResourceType[0] = typeResourceFood;
		hasResourceType[1] = typeResourceWood;
		hasResourceType[2] = typeResourceStone;
		hasResourceType[3] = typeResourceGold;
		if (typeResourceFood) { resourceTypes.push_back(0); }
		if (typeResourceWood) { resourceTypes.push_back(1); }
		if (typeResourceStone) { resourceTypes.push_back(2); }
		if (typeResourceGold) { resourceTypes.push_back(3); }
		parentType[cast(ParentBuildingType::OTHER)] = typeCenter || typeHome;
		parentType[cast(ParentBuildingType::DEFENCE)] = typeDefence;
		parentType[cast(ParentBuildingType::RESOURCE)]
			= typeResourceFood || typeResourceWood || typeResourceStone || typeResourceGold;
		parentType[cast(ParentBuildingType::TECHNOLOGY)] = typeTechBlacksmith || typeTechUniversity;
		parentType[cast(ParentBuildingType::UNITS)] = typeUnitBarracks || typeUnitRange || typeUnitCavalry;
	}

	std::optional<db_building_level*> getLevel(short level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}
};

struct db_building_level : db_with_name, db_with_cost, db_entity, db_level, db_base, db_building_attack,
                           db_with_model, db_build_upgrade {
	const short building;
	const short queueMaxCapacity;
	const float resourceRange;

	const Urho3D::String nodeName;

	std::vector<db_unit*> allUnits;

	std::vector<std::vector<db_unit*>*> unitsPerNation;
	std::vector<std::vector<unsigned char>*> unitsPerNationIds; //TODO remember must be storted

	//std::vector<db_building_metric*> dbBuildingMetricPerNation;
	db_building_metric* dbBuildingMetric;

	db_building_level(short id, short level, short building, char* name, char* nodeName, short queueMaxCapacity,
	                  short buildSpeed, short upgradeSpeed, short maxHp, float armor, float sightRng,
	                  float collect, float atckR, short atckRRld, float atckRRng, float resourceRng)
		: db_entity(id), db_level(level), db_with_name(name),
		  db_building_attack(collect, atckR, atckRRld, atckRRng),
		  db_base(maxHp, armor, sightRng), db_build_upgrade(buildSpeed, upgradeSpeed),
		  building(building), nodeName(nodeName), queueMaxCapacity(queueMaxCapacity), resourceRange(resourceRng) { }

	~db_building_level() {
		clear_vector(unitsPerNation);
		clear_vector(unitsPerNationIds);
		//clear_vector(dbBuildingMetricPerNation);
	}

	void finish(const std::vector<float>& newValues, const std::vector<float>& newValuesForSum,
	            std::vector<float>& valuesOtherNorm, std::vector<float>& valuesDefenceNorm,
	            std::vector<float>& valuesResourceNorm, std::vector<float>& valuesTechNorm,
	            std::vector<float>& valuesUnitsNorm) {
		dbBuildingMetric = new db_building_metric(newValues, newValuesForSum, valuesOtherNorm, valuesDefenceNorm,
		                                          valuesResourceNorm, valuesTechNorm, valuesUnitsNorm);
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
		  orderThresholdPrefix(split(orderThresholdPrefix, SPLIT_SIGN)) { }

	std::vector<std::string> splitAi(std::string* param) const {
		return split(split(param[id], SPLIT_SIGN_2)[SimGlobals::CURRENT_RUN], SPLIT_SIGN);
	}

	void refresh() {
		assert(id<MAX_PLAYERS); //TODO BUG to sa troszke inne rzeczy
		if (!SimGlobals::ACTION_AI_PATH[id].empty()) {
			actionPrefix = splitAi(SimGlobals::ACTION_AI_PATH);
		}
		if (!SimGlobals::ORDER_AI_PATH[id].empty()) {
			orderPrefix = splitAi(SimGlobals::ORDER_AI_PATH);
		}
		if (!SimGlobals::ORDER_THRESHOLD_PATH[id].empty()) {
			orderThresholdPrefix = splitAi(SimGlobals::ORDER_THRESHOLD_PATH);
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
		: db_entity(id), db_with_name(name), db_static({sizeX, sizeZ}), db_with_hp(maxHp, 0.f),
		  icon(icon),
		  nodeName(Urho3D::String(nodeName).Split(SPLIT_SIGN)),
		  maxUsers(maxUsers),
		  mini_map_color(mini_map_color) { }
};
