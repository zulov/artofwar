#include "db_struct.h"
#include "player/ai/MetricDefinitions.h"

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
