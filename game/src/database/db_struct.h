#pragma once
#include <algorithm>
#include <magic_enum.hpp>
#include <span>
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include <objects/resource/ResourceType.h>

#include "objects/building/ParentBuildingType.h"
#include "objects/unit/state/UnitState.h"
#include "db_basic_struct.h"
#include "db_columns.h"
#include "utils/SpanUtils.h"
#include "simulation/SimGlobals.h"
#include "utils/DeleteUtils.h"
#include "utils/OtherUtils.h"
#include "utils/StringUtils.h"
#include "db_utils.h"

struct db_building;
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
	const float bonusCavalry;
	const float bonusWorker;
	const float bonusSpecial;
	const float bonusMelee;
	const float bonusHeavy;
	const float bonusLight;
	const float bonusBuilding;

	db_unit_attack(float collect, float attack, short attackReload, short attackRange,
	               float bonusInfantry, float bonusRange, float bonusCavalry, float bonusWorker,
	               float bonusSpecial, float bonusMelee, float bonusHeavy, float bonusLight, float bonusBuilding)
		: db_common_attack(collect, attack, attackReload, attackRange),
		  bonusInfantry(bonusInfantry),
		  bonusRange(bonusRange),
		  bonusCavalry(bonusCavalry),
		  bonusWorker(bonusWorker),
		  bonusSpecial(bonusSpecial),
		  bonusMelee(bonusMelee),
		  bonusHeavy(bonusHeavy),
		  bonusLight(bonusLight),
		  bonusBuilding(bonusBuilding) {}
};

struct db_static {
	const Urho3D::UCharVector2 size;

	explicit db_static(const Urho3D::UCharVector2& size)
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
		for (size_t i = 0; i < newValues.size(); ++i) {
			assert(newValues[i] >= 0.f && newValues[i] <= 1.f
				&& "Metric value out of [0,1] range — adjust normalization weight");
		}
		float y = 1 / weightForSum;
		valuesNormForSum.resize(newValues.size());
		std::ranges::transform(newValues, valuesNormForSum.begin(), [y](float x){ return x * y; });

		valuesNorm.assign(newValues.begin(), newValues.end());
	}

protected:
	std::vector<float> valuesNorm;
	std::vector<float> typesNorm;

	std::vector<float> valuesNormForSum;

	void setVector(std::vector<float>& values, const std::span<const unsigned char>& idxs) {
		values.clear();
		values.reserve(idxs.size());
		for (const unsigned char idx : idxs) {
			values.push_back(valuesNorm[idx]);
		}
	}

public:
	const std::vector<float>& getValuesNorm() const { return valuesNorm; }
	const std::vector<float>& getValuesNormForSum() const { return valuesNormForSum; }

	std::span<const float> getTypesVal() const {
		return {typesNorm.data(), typesNorm.size()};
	}
};

struct db_building_metric : db_basic_metric {
	std::vector<float> otherNorm;
	std::vector<float> defenceNorm;
	std::vector<float> resourceNorm;
	std::vector<float> techNorm;
	std::vector<float> unitsNorm;

	db_building_metric(db_building* dbBuilding, db_building_level* dbLevel);

	const std::vector<float>& getValuesNormForType(ParentBuildingType type) const {
		switch (type) {
		case ParentBuildingType::OTHER:
			return otherNorm;
		case ParentBuildingType::DEFENCE:
			return defenceNorm;
		case ParentBuildingType::RESOURCE:
			return resourceNorm;
		case ParentBuildingType::TECH:
			return techNorm;
		case ParentBuildingType::UNITS:
			return unitsNorm;
		default:
			return typesNorm; //BUG? fallback - should all enum values be handled explicitly?
		}
	}
};

struct db_level {
	const unsigned char level;

	explicit db_level(unsigned char level) :
		level(level) {}
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
	db_unit_metric(db_unit* dbUnit, db_unit_level* dbLevel);
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
		: db_with_name(asUShort(s, C::id), asText(s, C::name)),
		  db_level(asUByte(s, C::level)),
		  db_with_cost(asUShort(s, C::food), asUShort(s, C::wood), asUShort(s, C::stone), asUShort(s, C::gold)),
		  db_unit_attack(asFloat(s, C::collect), asFloat(s, C::attack),
		                 asShort(s, C::attack_reload), asShort(s, C::attack_range),
		                 asFloat(s, C::bonus_infantry), asFloat(s, C::bonus_range),
		                 asFloat(s, C::bonus_cavalry), asFloat(s, C::bonus_worker),
		                 asFloat(s, C::bonus_special), asFloat(s, C::bonus_melee),
		                 asFloat(s, C::bonus_heavy), asFloat(s, C::bonus_light),
		                 asFloat(s, C::bonus_building)),
		  db_base(asUShort(s, C::max_hp), asFloat(s, C::armor), asFloat(s, C::sight_range)),
		  db_build_upgrade(asShort(s, C::build_time), asShort(s, C::upgrade_time)),
		  unit(asUShort(s, C::unit)),
		  minDist(asFloat(s, C::min_dist)),
		  maxSep(1.f),
		  mass(asFloat(s, C::mass)),
		  invMass(1.f / mass),
		  maxSpeed(asFloat(s, C::max_speed)),
		  minSpeed(asFloat(s, C::min_speed)),
		  maxForce(asUShort(s, C::max_force)),
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
	const bool typeCavalry;
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
		: db_with_icon(asUShort(s, C::id), asText(s, C::name), asText(s, C::icon)),
		  db_with_cost(asUShort(s, C::food), asUShort(s, C::wood), asUShort(s, C::stone), asUShort(s, C::gold)),
		  desiredState(UnitState(asByte(s, C::action_state))),
		  typeInfantry(asBool(s, C::type_infantry)),
		  typeRange(asBool(s, C::type_range)),
		  typeCavalry(asBool(s, C::type_cavalry)),
		  typeWorker(asBool(s, C::type_worker)),
		  typeSpecial(asBool(s, C::type_special)),
		  typeMelee(asBool(s, C::type_melee)),
		  typeHeavy(asBool(s, C::type_heavy)),
		  typeLight(asBool(s, C::type_light)) {}

	std::optional<db_unit_level*> getLevel(unsigned char level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}

	float getBonuses(db_unit_attack* dbLevel) const {
		return typeInfantry * dbLevel->bonusInfantry +
			typeRange * dbLevel->bonusRange +
			typeCavalry * dbLevel->bonusCavalry +
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

	// Cached lookup: everCanProduceUnitByNation[nationId][unitId] == 1 if any level can produce it.
	std::vector<std::vector<unsigned char>> everCanProduceUnitByNation;

	using C = DbBuildingCol;
	db_building(sqlite3_stmt* s)
		: db_with_icon(asUShort(s, C::id), asText(s, C::name), asText(s, C::icon)),
		  db_with_cost(asUShort(s, C::food), asUShort(s, C::wood), asUShort(s, C::stone), asUShort(s, C::gold)),
		db_static({asUByte(s, C::size_x), asUByte(s, C::size_z)}),
		  typeCenter(asBool(s, C::type_center)),
		  typeHome(asBool(s, C::type_home)),
		  typeDefence(asBool(s, C::type_defence)),
		  resourceType(asByte(s, C::type_resource)),
		  typeResourceFood(resourceType == cast(ResourceType::FOOD)),
		  typeResourceWood(resourceType == cast(ResourceType::WOOD)),
		  typeResourceStone(resourceType == cast(ResourceType::STONE)),
		  typeResourceGold(resourceType == cast(ResourceType::GOLD)),
		  typeTechBlacksmith(asBool(s, C::type_tech_blacksmith)),
		  typeTechUniversity(asBool(s, C::type_tech_university)),
		  typeUnitBarracks(asBool(s, C::type_unit_barracks)),
		  typeUnitRange(asBool(s, C::type_unit_range)),
		  typeUnitCavalry(asBool(s, C::type_unit_cavalry)),
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

	std::optional<db_building_level*> getLevel(unsigned short level) {
		if (levels.size() > level) {
			return levels.at(level);
		}
		return {};
	}

	void finish(size_t unitsCount, const std::vector<db_nation*>& allNations);

	bool canEverProduceUnit(unsigned char nationId, unsigned short unitId) const;

	bool hasResourceType(ResourceType resource) const { return resourceType == cast(resource); }
	bool isResourceBuilding() const { return typeResourceAny; }
	bool isResourceBonus(const db_building_level* level) const;
	bool isResourceBonus(const db_building_level* level, ResourceType resource) const;
	bool spawnsResource() const { return toResource >= 0; }
	bool spawnsResourceInPlace(const db_building_level* level) const;
	bool spawnsResourceNearby(const db_building_level* level) const;
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
	const short spawnResourceTime;
	const short spawnResourceRange;

	const Urho3D::String nodeName;

	std::vector<db_unit*> allUnits;

	std::vector<std::vector<db_unit*>*> unitsPerNation;
	std::vector<std::vector<unsigned short>*> unitsPerNationIds; //TODO remember must be storted

	//std::vector<db_building_metric*> dbBuildingMetricPerNation;
	db_building_metric* dbBuildingMetric;
	using C = DbBuildingLevelCol;
	db_building_level(sqlite3_stmt* s)
		: db_with_name(asUShort(s, C::id), asText(s, C::name)),
		  db_with_cost(asUShort(s, C::food), asUShort(s, C::wood), asUShort(s, C::stone), asUShort(s, C::gold)),
		  db_level(asUByte(s, C::level)),
		  db_base(asUShort(s, C::max_hp), asFloat(s, C::armor), asFloat(s, C::sight_range)),
		  db_building_attack(asFloat(s, C::collect), asFloat(s, C::attack),
		                     asShort(s, C::attack_reload), asShort(s, C::attack_range)),
		  db_build_upgrade(asShort(s, C::build_speed), asShort(s, C::upgrade_speed)),
		  building(asShort(s, C::building)),
		  queueMaxCapacity(asShort(s, C::queue_max_capacity)),
		  resourceRange(asFloat(s, C::resource_range)),
		  foodStorage(asShort(s, C::food_storage)),
		  goldStorage(asShort(s, C::gold_storage)),
		  stoneRefineCapacity(asFloat(s, C::stone_refine_capacity)),
		  goldRefineCapacity(asFloat(s, C::gold_refine_capacity)),
		  spawnResourceTime(asShort(s, C::spawn_resource_time)),
		  spawnResourceRange(asShort(s, C::spawn_resource_range)),
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

	bool isResourceBonus() const { return collect > 0.f && resourceRange > 0.f; }
	bool storesFood() const { return foodStorage > 0; }
	bool storesGold() const { return goldStorage > 0; }
	bool refinesStone() const { return stoneRefineCapacity > 0.f; }
	bool refinesGold() const { return goldRefineCapacity > 0.f; }
	bool spawnsResourceInPlace() const { return spawnResourceRange <= 0; }
	bool spawnsResourceNearby() const { return spawnResourceRange > 0; }
};

inline bool db_building::spawnsResourceInPlace(const db_building_level* level) const {
	return spawnsResource() && level->spawnsResourceInPlace();
}

inline bool db_building::spawnsResourceNearby(const db_building_level* level) const {
	return spawnsResource() && level->spawnsResourceNearby();
}

inline bool db_building::isResourceBonus(const db_building_level* level, ResourceType resource) const {
	return hasResourceType(resource) && level->isResourceBonus();
}

inline bool db_building::isResourceBonus(const db_building_level* level) const {
	return isResourceBuilding() && level->isResourceBonus();
}

struct db_nation : db_with_name {
	std::vector<db_unit*> units;
	std::vector<db_unit*> workers;
	std::vector<db_building*> buildings;

	std::vector<std::string> brainPrefix;

	db_nation(sqlite3_stmt* stmt) :
		db_with_name(asUShort(stmt, DbNationCol::id), asText(stmt, DbNationCol::name)),
		  brainPrefix(split(asText(stmt, DbNationCol::brain_prefix), SPLIT_SIGN)) {}

	std::vector<std::string> splitBrain(std::string* param) const {
		return split(split(param[id], SPLIT_SIGN_AI)[SimGlobals::CURRENT_RUN], SPLIT_SIGN);
	}

	void refresh() {
		assert(id<MAX_PLAYERS); //TODO BUG to sa troszke inne rzeczy
		if (!SimGlobals::BRAIN_AI_PATH[id].empty()) {
			brainPrefix = splitBrain(SimGlobals::BRAIN_AI_PATH);
		}
	}
};

inline void db_building::finish(size_t unitsCount, const std::vector<db_nation*>& allNations) {
	everCanProduceUnitByNation.assign(allNations.size(), std::vector<unsigned char>(unitsCount, 0));

	for (const auto nation : allNations) {
		if (nation == nullptr) { continue; }

		auto& cachedUnitFlags = everCanProduceUnitByNation[nation->id];

		for (const auto level : levels) {
			const auto* unitIds = level->unitsPerNationIds[nation->id];
			if (unitIds == nullptr) { continue; }

			for (const auto unitId : *unitIds) {
				if (unitId < cachedUnitFlags.size()) {
					cachedUnitFlags[unitId] = 1;
				}
			}
		}
	}
}

inline bool db_building::canEverProduceUnit(unsigned char nationId, unsigned short unitId) const {
	if (nationId >= everCanProduceUnitByNation.size()) { return false; }
	const auto& unitFlags = everCanProduceUnitByNation[nationId];
	return unitId < unitFlags.size() && unitFlags[unitId] != 0;
}

struct db_resource : db_with_icon, db_static, db_with_hp, db_with_model {
	const unsigned char resourceId;
	const unsigned char maxUsers;
	const unsigned mini_map_color;

	Urho3D::Vector<Urho3D::String> nodeName;
	const float collectSpeed;
	const bool rotatable;

	using C = DbResourceCol;
	db_resource(sqlite3_stmt* s)
		: db_with_icon(asUShort(s, C::id), asText(s, C::name), asText(s, C::icon)),
		db_static({asUByte(s, C::size_x), asUByte(s, C::size_z)}),
		  db_with_hp(asUShort(s, C::max_hp), 0.f),
		  resourceId(asUByte(s, C::resource_id)),
		  maxUsers(asUByte(s, C::max_users)),
		  mini_map_color(asHex(s, C::mini_map_color)),
		  nodeName(Urho3D::String(asText(s, C::node_name)).Split(SPLIT_SIGN)),
		  collectSpeed(asFloat(s, C::collect_speed)),
		  rotatable(asBool(s, C::rotatable)) {}
};
