#pragma once
#include <magic_enum.hpp>
#include <span>
#include <vector>
#include <valarray>
#include <Urho3D/Math/Vector2.h>
#include "objects/building/ParentBuildingType.h"
#include "objects/unit/state/UnitState.h"
#include "db_basic_struct.h"
#include "math/SpanUtils.h"
#include "simulation/SimGlobals.h"
#include "utils/DeleteUtils.h"
#include "utils/OtherUtils.h"
#include "utils/StringUtils.h"

constexpr char UNITS_SUM_X = 100;
constexpr char BUILDINGS_SUM_X = 10;

struct db_unit;
struct db_unit_metric;
struct db_nation;
struct db_building_level;
struct db_unit_level;


//TODO attack reload change to short from float chantge in db

float inline safeDiv(float first, short second) {
	second = second <= 0 ? 1 : second;
	return first > 0.f ? first / second * FRAMES_IN_PERIOD : 0.f;
}

struct db_common_attack {
	const float collect;
	const float attack;
	const short attackReload;
	const short attackRange;
	const short sqAttackRange;

	db_common_attack(float collect, float attack, short attackReload, short attackRange)
		: attack(attack), attackReload(attackReload), attackRange(attackRange), collect(collect),
		  sqAttackRange(attackRange * attackRange) {}

	bool initFlag(float val) const {
		return val > 0.f;
	}
};


struct db_building_attack : db_common_attack {
	const bool canAttack;

	db_building_attack(float collect, float attack, short attackReload, short attackRange)
		: db_common_attack(collect, attack, attackReload, attackRange),
		  canAttack(initFlag(attack)) {}
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

	db_unit_attack(float collect, float attack, short attackReload, short attackRange,
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
		  bonusBuilding(bonusBuilding) {}
};

struct db_static {
	const Urho3D::IntVector2 size;

	explicit db_static(const Urho3D::IntVector2& size)
		: size(size) {}
};

//TODO mem uprościć usunac tabele kosztowe
struct db_with_cost {
	const std::array<unsigned short, 4> values;

	const unsigned short maxFromWoodOrStone;
	const unsigned short sum = 0;
	const bool moreWoodThanStone;

	db_with_cost(unsigned short food, unsigned short wood, unsigned short stone, unsigned short gold) :
		values({food, wood, stone, gold}),
		maxFromWoodOrStone(std::max(values[1], values[2])),
		sum(food + wood + stone + gold),
		moreWoodThanStone(wood > stone) {
	}

	unsigned short getSumCost() const { return sum; }
};

struct db_basic_metric {
	db_basic_metric(const std::span<const float> newValues, float weightForSum) {
		assert(validateSpan(__LINE__, __FILE__, newValues));
		float y = 1 / weightForSum;
		valuesNormForSum.resize(newValues.size());
		std::ranges::transform(newValues, valuesNormForSum.begin(), [y](float x) { return x * y; });

		valuesNormAsVal = std::valarray(newValues.data(), newValues.size());
	}

protected:
	std::valarray<float> valuesNormAsVal;
	std::valarray<float> typesNormAsVal;

	std::vector<float> valuesNormForSum;

	void setValarray(std::valarray<float>& valarray, const std::span<const unsigned char>& idxs) {
		std::vector<float> temp;

		temp.reserve(idxs.size());
		for (const unsigned char idx : idxs) {
			temp.push_back(valuesNormAsVal[idx]);
		}
		valarray = std::valarray<float>(temp.data(), temp.size());
	}

public:
	//const std::vector<float>& getValuesNorm() const { return valuesNorm; }
	const std::valarray<float>& getValuesNormAsVal() const { return valuesNormAsVal; }
	const std::vector<float>& getValuesNormForSum() const { return valuesNormForSum; }

	const std::span<const float> getTypesVal() const {
		return std::span{std::begin(typesNormAsVal), typesNormAsVal.size()};
	}
};

struct db_building_metric : db_basic_metric {
	std::valarray<float> otherNormAsVal;
	std::valarray<float> defenceNormAsVal;
	std::valarray<float> resourceNormAsVal;
	std::valarray<float> techNormAsVal;
	std::valarray<float> unitsNormAsVal;

	db_building_metric(const std::span<const float> newValues,
	                   std::span<const unsigned char> otherIdxs, std::span<const unsigned char> defenceIdxs,
	                   std::span<const unsigned char> resourceIdxs, std::span<const unsigned char> techIdxs,
	                   std::span<const unsigned char> unitsIdxs,
	                   std::span<const unsigned char> typesIdxs) : db_basic_metric(newValues, BUILDINGS_SUM_X) {
		setValarray(otherNormAsVal, otherIdxs); //TODO bug czy to jest zainicjowane
		setValarray(defenceNormAsVal, defenceIdxs);
		setValarray(resourceNormAsVal, resourceIdxs);
		setValarray(techNormAsVal, techIdxs);
		setValarray(unitsNormAsVal, unitsIdxs);
		setValarray(typesNormAsVal, typesIdxs);
	}

	const std::valarray<float>& getValuesNormAsValForType(ParentBuildingType type) const {
		switch (type) {
		case ParentBuildingType::OTHER:
			return otherNormAsVal;
		case ParentBuildingType::DEFENCE:
			return defenceNormAsVal;
		case ParentBuildingType::RESOURCE:
			return resourceNormAsVal;
		case ParentBuildingType::TECH:
			return techNormAsVal;
		case ParentBuildingType::UNITS:
			return unitsNormAsVal;
		default: ;
		}
	}
};

struct db_level {
	const char level;

	explicit db_level(char level)
		: level(level) {}
};

struct db_with_hp {
	const unsigned short maxHp;
	const float invMaxHp;
	const float armor;

	explicit db_with_hp(unsigned short maxHp, float armor)
		: maxHp(maxHp), invMaxHp(1.f / maxHp), armor(armor) {}
};

struct db_base : db_with_hp {
	const float sightRadius;
	const float sqSightRadius;

	const float interestRange;
	const float sqInterestRange;

	db_base(unsigned short maxHp, float armor, float sightRadius)
		: db_with_hp(maxHp, armor), sightRadius(sightRadius), sqSightRadius(sightRadius * sightRadius),
		  interestRange(sightRadius * 0.8f), sqInterestRange(interestRange * interestRange) {}
};

struct db_with_model {
	float modelHeight = -1.f;
};

struct db_build_upgrade {
	short buildTime = -1;
	short upgradeTime = -1;

	db_build_upgrade(short buildTime, short upgradeTime)
		: buildTime(buildTime), upgradeTime(upgradeTime) {}
};

struct db_unit_metric : db_basic_metric {
	db_unit_metric(const std::span<const float> newValues, std::span<const unsigned char> typesIdxs) :
		db_basic_metric(newValues, UNITS_SUM_X) {
		setValarray(typesNormAsVal, typesIdxs);
	}
};

struct db_unit_level : db_with_name, db_level, db_with_cost, db_unit_attack, db_base, db_with_model,
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

	db_unit_level(short level, short unit, char* name, char* node,
	              unsigned short food, unsigned short wood, unsigned short stone, unsigned short gold,
	              short buildTime, short upgradeTime, float minDist, float mass, float minSpeed, float maxSpeed,
	              int maxForce, short maxHp, float armor, float sightRng, float collect, float atck, float atckRld,
	              short atckRng, float bI, float bR, float bC, float bW, float bS, float bM, float bH, float bL,
	              float bB) :
		db_with_name(-1, name), db_level(level), db_with_cost(food, wood, stone, gold),
		db_unit_attack(collect, atck, atckRld, atckRng, bI, bR, bC, bW, bS, bM, bH, bL, bB),
		db_base(maxHp, armor, sightRng), db_build_upgrade(buildTime, upgradeTime),
		unit(unit),
		minDist(minDist),
		maxSep(1.f),
		mass(mass),
		invMass(1 / mass),
		maxSpeed(maxSpeed),
		minSpeed(minSpeed),
		maxForce(maxForce),
		sqMinSpeed(minSpeed * minSpeed),
		node(node) {
	}

	void finish(const std::span<const float> newValues, std::span<const unsigned char> valuesUnitsTypesIdxs) {
		dbUnitMetric = new db_unit_metric(newValues, valuesUnitsTypesIdxs);
	}

	~db_unit_level() {
		delete dbUnitMetric;
	}
};

struct db_unit : db_with_icon, db_with_cost {
	const UnitState actionState;

	const bool typeInfantry;
	const bool typeRange;
	const bool typeCalvary;
	const bool typeWorker;
	const bool typeSpecial;
	const bool typeMelee;
	const bool typeHeavy;
	const bool typeLight;

	bool possibleStates[magic_enum::enum_count<UnitState>()];

	std::vector<db_unit_level*> levels; //todo array

	std::vector<db_nation*> nations;
	std::vector<unsigned char> ordersIds;

	db_unit(short id, char* name, char* icon,
	        unsigned short food, unsigned short wood, unsigned short stone, unsigned short gold,
	        char actionState, bool typeInfantry, bool typeRange, bool typeCalvary, bool typeWorker,
	        bool typeSpecial, bool typeMelee, bool typeHeavy, bool typeLight) ://TODO typeHeavy polaczyc z typeLight, usunac actionState?
		db_with_icon(id, name, icon), db_with_cost(food, wood, stone, gold),
		actionState(UnitState(actionState)),
		typeInfantry(typeInfantry),
		typeRange(typeRange),
		typeCalvary(typeCalvary),
		typeWorker(typeWorker),
		typeSpecial(typeSpecial),
		typeMelee(typeMelee),
		typeHeavy(typeHeavy),
		typeLight(typeLight) {}

	std::optional<db_unit_level*> getLevel(char level) {
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


struct db_building : db_with_icon, db_with_cost, db_static {
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

	//TODO init
	bool typeResourceFoodBonus;
	bool typeResourceWoodBonus;
	bool typeResourceStoneBonus;
	bool typeResourceGoldBonus;

	bool ruinable;
	short toResource;

	unsigned char maxUsers;
	bool typeResourceAny;
	char resourceType;

	bool parentType[magic_enum::enum_count<ParentBuildingType>()];

	std::vector<db_building_level*> levels;

	std::vector<db_nation*> nations;


	db_building(short id, char* name, char* icon,
	            unsigned short food, unsigned short wood, unsigned short stone, unsigned short gold,
	            short sizeX, short sizeZ, bool typeCenter, bool typeHome, bool typeDefence, char typeResource,
	            bool typeTechBlacksmith, bool typeTechUniversity, bool typeUnitBarracks, bool typeUnitRange,
	            bool typeUnitCavalry, bool ruinable, short toResource)
		: db_with_icon(id, name, icon), db_with_cost(food, wood, stone, gold), db_static({sizeX, sizeZ}),
		  typeCenter(typeCenter), typeHome(typeHome), typeDefence(typeDefence), resourceType(typeResource),
		  typeResourceFood(typeResource == 0), typeResourceWood(typeResource == 1),
		  typeResourceStone(typeResource == 2), typeResourceGold(typeResource == 3),
		  typeTechBlacksmith(typeTechBlacksmith), typeTechUniversity(typeTechUniversity),
		  typeUnitBarracks(typeUnitBarracks), typeUnitRange(typeUnitRange), typeUnitCavalry(typeUnitCavalry),
		  ruinable(ruinable), toResource(toResource), maxUsers(sizeX * 2 + sizeZ * 2 + 4),
		  typeResourceAny(typeResource >= 0) {
		parentType[cast(ParentBuildingType::OTHER)] = typeCenter || typeHome;
		parentType[cast(ParentBuildingType::DEFENCE)] = typeDefence;
		parentType[cast(ParentBuildingType::RESOURCE)]
			= typeResourceFood || typeResourceWood || typeResourceStone || typeResourceGold;
		parentType[cast(ParentBuildingType::TECH)] = typeTechBlacksmith || typeTechUniversity;
		parentType[cast(ParentBuildingType::UNITS)] = typeUnitBarracks || typeUnitRange || typeUnitCavalry;
	}

	std::optional<db_building_level*> getLevel(short level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}
};

struct db_building_level : db_with_name, db_with_cost, db_level, db_base, db_building_attack,
                           db_with_model, db_build_upgrade {
	const short building;
	const short queueMaxCapacity;
	const float resourceRange;
	const short foodStorage;
	const short goldStoreage;
	const float stoneRefineCapacity;
	const float goldRefineCapacity;

	const Urho3D::String nodeName;

	std::vector<db_unit*> allUnits;

	std::vector<std::vector<db_unit*>*> unitsPerNation;
	std::vector<std::vector<unsigned char>*> unitsPerNationIds; //TODO remember must be storted

	//std::vector<db_building_metric*> dbBuildingMetricPerNation;
	db_building_metric* dbBuildingMetric;

	db_building_level(short id, short level, short building, char* name, char* nodeName,
	                  unsigned short food, unsigned short wood, unsigned short stone, unsigned short gold,
	                  short queueMaxCapacity, short buildSpeed, short upgradeSpeed, short maxHp, float armor,
	                  float sightRng, float collect, float atckR, short atckRRld, float atckRRng, float resourceRng,
	                  short foodStorage, short goldStoreage, float stoneRefineCapacity, float goldRefineCapacity)
		: db_with_name(id, name), db_with_cost(food, wood, stone, gold), db_level(level),
		  db_building_attack(collect, atckR, atckRRld, atckRRng),
		  db_base(maxHp, armor, sightRng), db_build_upgrade(buildSpeed, upgradeSpeed),
		  building(building), nodeName(nodeName), queueMaxCapacity(queueMaxCapacity), resourceRange(resourceRng),
		  foodStorage(foodStorage), goldStoreage(goldStoreage), stoneRefineCapacity(stoneRefineCapacity),
		  goldRefineCapacity(goldRefineCapacity) {
	}

	~db_building_level() {
		clear_vector(unitsPerNation);
		clear_vector(unitsPerNationIds);
		delete dbBuildingMetric;
		//clear_vector(dbBuildingMetricPerNation);
	}

	void finish(const std::span<const float> newValues,
	            std::span<const unsigned char> valuesOtherIdxs, std::span<const unsigned char> valuesDefenceIdxs,
	            std::span<const unsigned char> valuesResourceIdxs, std::span<const unsigned char> valuesTechIdxs,
	            std::span<const unsigned char> valuesUnitsIdxs, std::span<const unsigned char> typesIdxs) {
		dbBuildingMetric = new db_building_metric(newValues, valuesOtherIdxs, valuesDefenceIdxs,
		                                          valuesResourceIdxs, valuesTechIdxs, valuesUnitsIdxs, typesIdxs);
	}
};

struct db_nation : db_with_name {
	std::vector<db_unit*> units;
	std::vector<db_unit*> workers;
	std::vector<db_building*> buildings;

	std::vector<std::string> actionPrefix;
	std::vector<std::string> orderPrefix;

	db_nation(short id, char* name, char* actionPrefix, char* orderPrefix)
		: db_with_name(id, name),
		  actionPrefix(split(actionPrefix, SPLIT_SIGN)),
		  orderPrefix(split(orderPrefix, SPLIT_SIGN)) {}

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
	}
};

struct db_resource : db_with_icon, db_static, db_with_hp, db_with_model {
	const char resourceId;
	const short maxUsers;
	const unsigned mini_map_color;

	Urho3D::Vector<Urho3D::String> nodeName;
	const float collectSpeed;
	const bool rotatable;

	db_resource(short id, char resourceId, char* name, char* icon, unsigned short maxHp, char* nodeName, short sizeX,
	            short sizeZ, short maxUsers, unsigned mini_map_color, float collectSpeed, bool rotatable)
		: db_with_icon(id, name, icon), db_static({sizeX, sizeZ}), db_with_hp(maxHp, 0.f),
		  resourceId(resourceId), nodeName(Urho3D::String(nodeName).Split(SPLIT_SIGN)),
		  maxUsers(maxUsers), mini_map_color(mini_map_color), collectSpeed(collectSpeed), rotatable(rotatable) {}
};
