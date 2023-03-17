#include "ObjectsInfo.h"

ObjectsInfo::ObjectsInfo() {
	reset();
}

void ObjectsInfo::reset() {
	unitDied = false;
	amountUnitChanged = false;

	buildingDied = false;
	amountBuildingChanged = false;

	resourceDied = false;
	amountResourceChanged = false;
}

void ObjectsInfo::setUnitDied() {
	unitDied = true;
	amountUnitChanged = true;
}

void ObjectsInfo::setAmountUnitChanged() {
	amountUnitChanged = true;
}

void ObjectsInfo::setBuildingDied() {
	buildingDied = true;
	amountBuildingChanged = true;
}

void ObjectsInfo::setAmountBuildingChanged() {
	amountBuildingChanged = true;
}

void ObjectsInfo::setResourceDied() {
	resourceDied = true;
	amountResourceChanged = true;
}

void ObjectsInfo::setAmountResourceChanged() {
	amountResourceChanged = true;
}
