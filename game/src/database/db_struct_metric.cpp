#include "db_struct.h"
#include "player/ai/MetricDefinitions.h"

using U = MetricDefinitions::U;
using B = MetricDefinitions::B;

std::array<AiUnitMetric, MetricDefinitions::UNIT_METRIC_COUNT> MetricDefinitions::aiUnitMetric = buildMetricArray<U, AiUnitMetric>({
	{U::COST,            {[](db_unit* u, db_unit_level* l) -> float { return u->getSumCost(); }, 500}},
	{U::MAX_HP,          {[](db_unit* u, db_unit_level* l) -> float { return l->maxHp; }, 500}},
	{U::ARMOR,           {[](db_unit* u, db_unit_level* l) -> float { return l->armor; }}},
	{U::SIGHT_RADIUS,    {[](db_unit* u, db_unit_level* l) -> float { return l->sightRadius; }, 20}},
	{U::ATTACK,          {[](db_unit* u, db_unit_level* l) -> float { return l->attack; }, 10}},
	{U::ATTACK_RELOAD,   {[](db_unit* u, db_unit_level* l) -> float { return l->attackReload; }, 200}},
	{U::ATTACK_RANGE,    {[](db_unit* u, db_unit_level* l) -> float { return l->attackRange; }, 20}},
	{U::TYPE_INFANTRY,   {[](db_unit* u, db_unit_level* l) -> float { return u->typeInfantry; }}},
	{U::TYPE_RANGE,      {[](db_unit* u, db_unit_level* l) -> float { return u->typeRange; }}},
	{U::TYPE_CAVALRY,    {[](db_unit* u, db_unit_level* l) -> float { return u->typeCavalry; }}},
	{U::TYPE_SPECIAL,    {[](db_unit* u, db_unit_level* l) -> float { return u->typeSpecial; }}},
	{U::TYPE_MELEE,      {[](db_unit* u, db_unit_level* l) -> float { return u->typeMelee; }}},
	{U::TYPE_HEAVY,      {[](db_unit* u, db_unit_level* l) -> float { return u->typeHeavy; }}},
	{U::TYPE_LIGHT,      {[](db_unit* u, db_unit_level* l) -> float { return u->typeLight; }}},
	{U::BONUS_INFANTRY,  {[](db_unit* u, db_unit_level* l) -> float { return l->bonusInfantry; }, 3}},
	{U::BONUS_RANGE,     {[](db_unit* u, db_unit_level* l) -> float { return l->bonusRange; }, 3}},
	{U::BONUS_CAVALRY,   {[](db_unit* u, db_unit_level* l) -> float { return l->bonusCavalry; }, 3}},
	{U::BONUS_WORKER,    {[](db_unit* u, db_unit_level* l) -> float { return l->bonusWorker; }, 3}},
	{U::BONUS_SPECIAL,   {[](db_unit* u, db_unit_level* l) -> float { return l->bonusSpecial; }, 3}},
	{U::BONUS_MELEE,     {[](db_unit* u, db_unit_level* l) -> float { return l->bonusMelee; }, 3}},
	{U::BONUS_HEAVY,     {[](db_unit* u, db_unit_level* l) -> float { return l->bonusHeavy; }, 3}},
	{U::BONUS_LIGHT,     {[](db_unit* u, db_unit_level* l) -> float { return l->bonusLight; }, 3}},
	{U::BONUS_BUILDING,  {[](db_unit* u, db_unit_level* l) -> float { return l->bonusBuilding; }, 3}},
});

std::array<AiBuildingMetric, MetricDefinitions::BUILDING_METRIC_COUNT> MetricDefinitions::aiBuildingMetric = buildMetricArray<B, AiBuildingMetric>({
	{B::COST,               {[](db_building* b, db_building_level* l) -> float { return b->getSumCost(); }, 1000}},
	{B::MAX_HP,             {[](db_building* b, db_building_level* l) -> float { return l->maxHp; }, 5000}},
	{B::ARMOR,              {[](db_building* b, db_building_level* l) -> float { return l->armor; }}},
	{B::SIGHT_RADIUS,       {[](db_building* b, db_building_level* l) -> float { return l->sightRadius; }, 50}},
	{B::COLLECT,            {[](db_building* b, db_building_level* l) -> float { return l->collect; }, 2}},
	{B::ATTACK,             {[](db_building* b, db_building_level* l) -> float { return l->attack; }, 20}},
	{B::ATTACK_RELOAD,      {[](db_building* b, db_building_level* l) -> float { return l->attackReload; }, 200}},
	{B::ATTACK_RANGE,       {[](db_building* b, db_building_level* l) -> float { return l->attackRange; }, 30}},
	{B::RESOURCE_RANGE,     {[](db_building* b, db_building_level* l) -> float { return l->resourceRange; }, 20}},
	{B::TYPE_CENTER,        {[](db_building* b, db_building_level* l) -> float { return b->typeCenter; }}},
	{B::TYPE_HOME,          {[](db_building* b, db_building_level* l) -> float { return b->typeHome; }}},
	{B::TYPE_DEFENCE,       {[](db_building* b, db_building_level* l) -> float { return b->typeDefence; }}},
	{B::TYPE_RES_FOOD,      {[](db_building* b, db_building_level* l) -> float { return b->typeResourceFood; }}},
	{B::TYPE_RES_WOOD,      {[](db_building* b, db_building_level* l) -> float { return b->typeResourceWood; }}},
	{B::TYPE_RES_STONE,     {[](db_building* b, db_building_level* l) -> float { return b->typeResourceStone; }}},
	{B::TYPE_RES_GOLD,      {[](db_building* b, db_building_level* l) -> float { return b->typeResourceGold; }}},
	{B::TYPE_TECH_BLACKSMITH, {[](db_building* b, db_building_level* l) -> float { return b->typeTechBlacksmith; }}},
	{B::TYPE_TECH_UNIVERSITY, {[](db_building* b, db_building_level* l) -> float { return b->typeTechUniversity; }}},
	{B::TYPE_UNIT_BARRACKS, {[](db_building* b, db_building_level* l) -> float { return b->typeUnitBarracks; }}},
	{B::TYPE_UNIT_RANGE,    {[](db_building* b, db_building_level* l) -> float { return b->typeUnitRange; }}},
	{B::TYPE_UNIT_CAVALRY,  {[](db_building* b, db_building_level* l) -> float { return b->typeUnitCavalry; }}},
});

std::span<const float> MetricDefinitions::writeUnit(db_unit* unit, db_unit_level* level) const {
	return compose(section(aiUnitMetric, unit, level));
}

std::span<const float> MetricDefinitions::writeBuilding(db_building* building, db_building_level* level) const {
	return compose(section(aiBuildingMetric, building, level));
}

db_building_metric::db_building_metric(db_building* dbBuilding, db_building_level* dbLevel) :
	db_basic_metric(METRIC_DEFINITIONS.writeBuilding(dbBuilding, dbLevel), BUILDINGS_SUM_X) {
	setValarray(otherNormAsVal, METRIC_DEFINITIONS.getBuildingOtherIdxs());
	setValarray(defenceNormAsVal, METRIC_DEFINITIONS.getBuildingDefenceIdxs());
	setValarray(resourceNormAsVal, METRIC_DEFINITIONS.getBuildingResourceIdxs());
	setValarray(techNormAsVal, METRIC_DEFINITIONS.getBuildingTechIdxs());
	setValarray(unitsNormAsVal, METRIC_DEFINITIONS.getBuildingUnitsIdxs());
	setValarray(typesNormAsVal, METRIC_DEFINITIONS.getBuildingTypesIdxs());
}

db_unit_metric::db_unit_metric(db_unit* dbUnit, db_unit_level* dbLevel) :
	db_basic_metric(METRIC_DEFINITIONS.writeUnit(dbUnit, dbLevel), UNITS_SUM_X) {
	setValarray(typesNormAsVal, METRIC_DEFINITIONS.getUnitTypesIdxs());
}
