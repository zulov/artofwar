#include "db_struct.h"
#include "player/ai/MetricDefinitions.h"

using U = MetricDefinitions::U;
using B = MetricDefinitions::B;

std::array<AiUnitMetric, MetricDefinitions::UNIT_METRIC_COUNT> MetricDefinitions::aiUnitMetric = buildMetricArray<U, AiUnitMetric>({
	{U::COST,            {[](auto u, auto l) -> float { return u->getSumCost(); }, 500}},
	{U::MAX_HP,          {[](auto u, auto l) -> float { return l->maxHp; }, 500}},
	{U::ARMOR,           {[](auto u, auto l) -> float { return l->armor; }}},
	{U::SIGHT_RADIUS,    {[](auto u, auto l) -> float { return l->sightRadius; }, 20}},
	{U::ATTACK,          {[](auto u, auto l) -> float { return l->attack; }, 10}},
	{U::ATTACK_RELOAD,   {[](auto u, auto l) -> float { return l->attackReload; }, 200}},
	{U::ATTACK_RANGE,    {[](auto u, auto l) -> float { return l->attackRange; }, 20}},
	{U::TYPE_INFANTRY,   {[](auto u, auto l) -> float { return u->typeInfantry; }}},
	{U::TYPE_RANGE,      {[](auto u, auto l) -> float { return u->typeRange; }}},
	{U::TYPE_CAVALRY,    {[](auto u, auto l) -> float { return u->typeCavalry; }}},
	{U::TYPE_SPECIAL,    {[](auto u, auto l) -> float { return u->typeSpecial; }}},
	{U::TYPE_MELEE,      {[](auto u, auto l) -> float { return u->typeMelee; }}},
	{U::TYPE_HEAVY,      {[](auto u, auto l) -> float { return u->typeHeavy; }}},
	{U::TYPE_LIGHT,      {[](auto u, auto l) -> float { return u->typeLight; }}},
	{U::BONUS_INFANTRY,  {[](auto u, auto l) -> float { return l->bonusInfantry; }, 3}},
	{U::BONUS_RANGE,     {[](auto u, auto l) -> float { return l->bonusRange; }, 3}},
	{U::BONUS_CAVALRY,   {[](auto u, auto l) -> float { return l->bonusCavalry; }, 3}},
	{U::BONUS_WORKER,    {[](auto u, auto l) -> float { return l->bonusWorker; }, 3}},
	{U::BONUS_SPECIAL,   {[](auto u, auto l) -> float { return l->bonusSpecial; }, 3}},
	{U::BONUS_MELEE,     {[](auto u, auto l) -> float { return l->bonusMelee; }, 3}},
	{U::BONUS_HEAVY,     {[](auto u, auto l) -> float { return l->bonusHeavy; }, 3}},
	{U::BONUS_LIGHT,     {[](auto u, auto l) -> float { return l->bonusLight; }, 3}},
	{U::BONUS_BUILDING,  {[](auto u, auto l) -> float { return l->bonusBuilding; }, 3}},
});

std::array<AiBuildingMetric, MetricDefinitions::BUILDING_METRIC_COUNT> MetricDefinitions::aiBuildingMetric = buildMetricArray<B, AiBuildingMetric>({
	{B::COST,               {[](auto b, auto l) -> float { return b->getSumCost(); }, 1000}},
	{B::MAX_HP,             {[](auto b, auto l) -> float { return l->maxHp; }, 5000}},
	{B::ARMOR,              {[](auto b, auto l) -> float { return l->armor; }}},
	{B::SIGHT_RADIUS,       {[](auto b, auto l) -> float { return l->sightRadius; }, 50}},
	{B::COLLECT,            {[](auto b, auto l) -> float { return l->collect; }, 2}},
	{B::ATTACK,             {[](auto b, auto l) -> float { return l->attack; }, 20}},
	{B::ATTACK_RELOAD,      {[](auto b, auto l) -> float { return l->attackReload; }, 200}},
	{B::ATTACK_RANGE,       {[](auto b, auto l) -> float { return l->attackRange; }, 30}},
	{B::RESOURCE_RANGE,     {[](auto b, auto l) -> float { return l->resourceRange; }, 20}},
	{B::TYPE_CENTER,        {[](auto b, auto l) -> float { return b->typeCenter; }}},
	{B::TYPE_HOME,          {[](auto b, auto l) -> float { return b->typeHome; }}},
	{B::TYPE_DEFENCE,       {[](auto b, auto l) -> float { return b->typeDefence; }}},
	{B::TYPE_RES_FOOD,      {[](auto b, auto l) -> float { return b->typeResourceFood; }}},
	{B::TYPE_RES_WOOD,      {[](auto b, auto l) -> float { return b->typeResourceWood; }}},
	{B::TYPE_RES_STONE,     {[](auto b, auto l) -> float { return b->typeResourceStone; }}},
	{B::TYPE_RES_GOLD,      {[](auto b, auto l) -> float { return b->typeResourceGold; }}},
	{B::TYPE_TECH_BLACKSMITH, {[](auto b, auto l) -> float { return b->typeTechBlacksmith; }}},
	{B::TYPE_TECH_UNIVERSITY, {[](auto b, auto l) -> float { return b->typeTechUniversity; }}},
	{B::TYPE_UNIT_BARRACKS, {[](auto b, auto l) -> float { return b->typeUnitBarracks; }}},
	{B::TYPE_UNIT_RANGE,    {[](auto b, auto l) -> float { return b->typeUnitRange; }}},
	{B::TYPE_UNIT_CAVALRY,  {[](auto b, auto l) -> float { return b->typeUnitCavalry; }}},
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
