#pragma once
#include <magic_enum.hpp>
#include <span>
#include <vector>
#include <valarray>
#include <Urho3D/Math/Vector2.h>
#include "objects/building/ParentBuildingType.h"
#include "objects/unit/state/UnitState.h"
#include "db_basic_struct.h"
#include "db_columns.h"
#include "utils/SpanUtils.h"
#include "player/ai/MetricDefinitions.h"
#include "simulation/SimGlobals.h"
#include "utils/DeleteUtils.h"
#include "utils/OtherUtils.h"
#include "utils/StringUtils.h"
#include "db_utils.h"

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
		: collect(collect), attack(attack), attackReload(attackReload), attackRange(attackRange),
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
		maxFromWoodOrStone(std::max(values[cast(ResourceType::WOOD)], values[cast(ResourceType::STONE)])),
		sum(food + wood + stone + gold),
		moreWoodThanStone(wood > stone) {}

	unsigned short getSumCost() const { return sum; }
};

struct db_basic_metric {
	db_basic_metric(const std::span<const float> newValues, float weightForSum) {
		assert(validateSpan(__LINE__, __FILE__, newValues));
		float y = 1 / weightForSum;
		valuesNormForSum.resize(newValues.size());
		std::ranges::transform(newValues, valuesNormForSum.begin(), [y](float x){ return x * y; });

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

	db_building_metric(db_building* dbBuilding, db_building_level* dbLevel) :
		db_basic_metric(METRIC_DEFINITIONS.writeBuilding(dbBuilding, dbLevel), BUILDINGS_SUM_X) {
		setValarray(otherNormAsVal, METRIC_DEFINITIONS.getBuildingOtherIdxs()); //TODO bug czy to jest zainicjowane
		setValarray(defenceNormAsVal, METRIC_DEFINITIONS.getBuildingDefenceIdxs());
		setValarray(resourceNormAsVal, METRIC_DEFINITIONS.getBuildingResourceIdxs());
		setValarray(techNormAsVal, METRIC_DEFINITIONS.getBuildingTechIdxs());
		setValarray(unitsNormAsVal, METRIC_DEFINITIONS.getBuildingUnitsIdxs());
		setValarray(typesNormAsVal, METRIC_DEFINITIONS.getBuildingTypesIdxs());
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
		default:
			return typesNormAsVal; //BUG? fallback - should all enum values be handled explicitly?
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
	db_unit_metric(db_unit* dbUnit, db_unit_level* dbLevel) :
		db_basic_metric(METRIC_DEFINITIONS.writeUnit(dbUnit, dbLevel), UNITS_SUM_X) {
		setValarray(typesNormAsVal, METRIC_DEFINITIONS.getUnitTypesIdxs());
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

	db_unit_metric* dbUnitMetric = nullptr;

	using C = DbUnitLevelCol;
	db_unit_level(sqlite3_stmt* s)
		: db_with_name(asShort(s, C::id), asText(s, C::name)),
		  db_level(asShort(s, C::level)),
		  db_with_cost(asUS(s, C::food), asUS(s, C::wood), asUS(s, C::stone), asUS(s, C::gold)),
		  db_unit_attack(asFloat(s, C::collect), asFloat(s, C::attack),
		                 asFloat(s, C::attack_reload), asShort(s, C::attack_range),
		                 asFloat(s, C::bonus_infantry), asFloat(s, C::bonus_range),
		                 asFloat(s, C::bonus_calvary), asFloat(s, C::bonus_worker),
		                 asFloat(s, C::bonus_special), asFloat(s, C::bonus_melee),
		                 asFloat(s, C::bonus_heavy), asFloat(s, C::bonus_light),
		                 asFloat(s, C::bonus_building)),
		  db_base(asShort(s, C::max_hp), asFloat(s, C::armor), asFloat(s, C::sight_range)),
		  db_build_upgrade(asShort(s, C::build_time), asShort(s, C::upgrade_time)),
		  unit(asShort(s, C::unit)),
		  minDist(asFloat(s, C::min_dist)),
		  maxSep(1.f),
		  mass(asFloat(s, C::mass)),
		  invMass(1.f / mass),
		  maxSpeed(asFloat(s, C::max_speed)),
		  minSpeed(asFloat(s, C::min_speed)),
		  maxForce(asShort(s, C::max_force)),
		  sqMinSpeed(minSpeed * minSpeed),
		  node(asText(s, C::node)) {}

	void finish(db_unit* dbUnit) {
		dbUnitMetric = new db_unit_metric(dbUnit, this);
	}

	~db_unit_level() {
		delete dbUnitMetric;
	}
};

struct db_unit : db_with_icon, db_with_cost {
	const UnitState desiredState;

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

	using C = DbUnitCol;
	db_unit(sqlite3_stmt* s)
		: db_with_icon(asShort(s, C::id), asText(s, C::name), asText(s, C::icon)),
		  db_with_cost(asUS(s, C::food), asUS(s, C::wood), asUS(s, C::stone), asUS(s, C::gold)),
		  desiredState(UnitState(asByte(s, C::action_state))),
		  typeInfantry(asBool(s, C::type_infantry)),
		  typeRange(asBool(s, C::type_range)),
		  typeCalvary(asBool(s, C::type_calvary)),
		  typeWorker(asBool(s, C::type_worker)),
		  typeSpecial(asBool(s, C::type_special)),
		  typeMelee(asBool(s, C::type_melee)),
		  typeHeavy(asBool(s, C::type_heavy)),
		  typeLight(asBool(s, C::type_light)) {}

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

	char resourceType;

	bool typeResourceFood;
	bool typeResourceWood;
	bool typeResourceStone;
	bool typeResourceGold;

	bool typeTechBlacksmith;
	bool typeTechUniversity;

	bool typeUnitBarracks; //IDEA moze te trzy w polaczyc w bazie?
	bool typeUnitRange;
	bool typeUnitCavalry;

	//TODO init - initialized to false for now, set properly when bonus resource buildings are added
	bool typeResourceFoodBonus = false;
	bool typeResourceWoodBonus = false;
	bool typeResourceStoneBonus = false;
	bool typeResourceGoldBonus = false;

	bool ruinable;
	short toResource;//ToDO to może dać do levelu

	unsigned char maxUsers;
	bool typeResourceAny;

	bool parentType[magic_enum::enum_count<ParentBuildingType>()];

	std::vector<db_building_level*> levels;

	std::vector<db_nation*> nations;

	using C = DbBuildingCol;
	db_building(sqlite3_stmt* s)
		: db_with_icon(asShort(s, C::id), asText(s, C::name), asText(s, C::icon)),
		  db_with_cost(asUS(s, C::food), asUS(s, C::wood), asUS(s, C::stone), asUS(s, C::gold)),
		  db_static({asShort(s, C::size_x), asShort(s, C::size_z)}),
		  typeCenter(asBool(s, C::type_center)),
		  typeHome(asBool(s, C::type_home)),
		  typeDefence(asBool(s, C::type_defence)),
		  typeResourceFood(resourceType == cast(ResourceType::FOOD)),
		  typeResourceWood(resourceType == cast(ResourceType::WOOD)),
		  typeResourceStone(resourceType == cast(ResourceType::STONE)),
		  typeResourceGold(resourceType == cast(ResourceType::GOLD)),
		  typeTechBlacksmith(asBool(s, C::type_tech_blacksmith)),
		  typeTechUniversity(asBool(s, C::type_tech_university)),
		  typeUnitBarracks(asBool(s, C::type_unit_barracks)),
		  typeUnitRange(asBool(s, C::type_unit_range)),
		  typeUnitCavalry(asBool(s, C::type_unit_cavalry)),
		  resourceType(asShort(s, C::type_resource)),
		  ruinable(asBool(s, C::ruinable)),
		  toResource(asShort(s, C::to_resource)),
		  maxUsers(size.x_ * 2 + size.y_ * 2 + 4),
		  typeResourceAny(resourceType >= 0) {
		parentType[castC(ParentBuildingType::OTHER)] = typeCenter || typeHome;
		parentType[castC(ParentBuildingType::DEFENCE)] = typeDefence;
		parentType[castC(ParentBuildingType::RESOURCE)]
			= typeResourceFood || typeResourceWood || typeResourceStone || typeResourceGold;
		parentType[castC(ParentBuildingType::TECH)] = typeTechBlacksmith || typeTechUniversity;
		parentType[castC(ParentBuildingType::UNITS)] = typeUnitBarracks || typeUnitRange || typeUnitCavalry;
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
	const short goldStorage;
	const float stoneRefineCapacity;
	const float goldRefineCapacity;
	const int spawnResourceTime;
	const int spawnResourceRange;

	const Urho3D::String nodeName;

	std::vector<db_unit*> allUnits;

	std::vector<std::vector<db_unit*>*> unitsPerNation;
	std::vector<std::vector<unsigned char>*> unitsPerNationIds; //TODO remember must be storted

	//std::vector<db_building_metric*> dbBuildingMetricPerNation;
	db_building_metric* dbBuildingMetric;
	using C = DbBuildingLevelCol;
	db_building_level(sqlite3_stmt* s)
		: db_with_name(asShort(s, C::id), asText(s, C::name)),
		  db_with_cost(asUS(s, C::food), asUS(s, C::wood), asUS(s, C::stone), asUS(s, C::gold)),
		  db_level(asShort(s, C::level)),
		  db_base(asShort(s, C::max_hp), asFloat(s, C::armor), asFloat(s, C::sight_range)),
		  db_building_attack(asFloat(s, C::collect), asFloat(s, C::attack),
		                     asShort(s, C::attack_reload), asFloat(s, C::attack_range)),
		  db_build_upgrade(asShort(s, C::build_speed), asShort(s, C::upgrade_speed)),
		  building(asShort(s, C::building)),
		  queueMaxCapacity(asShort(s, C::queue_max_capacity)),
		  resourceRange(asFloat(s, C::resource_range)),
		  foodStorage(asShort(s, C::food_storage)),
		  goldStorage(asShort(s, C::gold_storage)),
		  stoneRefineCapacity(asFloat(s, C::stone_refine_capacity)),
		  goldRefineCapacity(asFloat(s, C::gold_refine_capacity)),
		  spawnResourceTime(asInt(s, C::spawn_resource_time)),
		  spawnResourceRange(asInt(s, C::spawn_resource_range)),
		  nodeName(asText(s, C::node_name)) {}

	~db_building_level() {
		clear_vector(unitsPerNation);
		clear_vector(unitsPerNationIds);
		delete dbBuildingMetric;
		//clear_vector(dbBuildingMetricPerNation);
	}

	void finish(db_building* dbBuilding) {
		dbBuildingMetric = new db_building_metric(dbBuilding, this);
	}
};

struct db_nation : db_with_name {
	std::vector<db_unit*> units;
	std::vector<db_unit*> workers;
	std::vector<db_building*> buildings;

	std::vector<std::string> actionPrefix;
	std::vector<std::string> orderPrefix;

	db_nation(sqlite3_stmt* stmt)
		: db_with_name(asShort(stmt, DbNationCol::id), asText(stmt, DbNationCol::name)),
		  actionPrefix(split(asText(stmt, DbNationCol::action_prefix), SPLIT_SIGN)),
		  orderPrefix(split(asText(stmt, DbNationCol::order_prefix), SPLIT_SIGN)) {}

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

	using C = DbResourceCol;
	db_resource(sqlite3_stmt* s)
		: db_with_icon(asShort(s, C::id), asText(s, C::name), asText(s, C::icon)),
		  db_static({asShort(s, C::size_x), asShort(s, C::size_z)}),
		  db_with_hp(asUS(s, C::max_hp), 0.f),
		  resourceId(asByte(s, C::resource_id)),
		  maxUsers(asShort(s, C::max_users)),
		  mini_map_color(asHex(s, C::mini_map_color)),
		  nodeName(Urho3D::String(asText(s, C::node_name)).Split(SPLIT_SIGN)),
		  collectSpeed(asFloat(s, C::collect_speed)),
		  rotatable(asBool(s, C::rotatable)) {}
};
