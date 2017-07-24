#include "Building.h"
#include "ObjectEnums.h"
#include "db_strcut.h"

double Building::hbMaxSize = 5.0;
Building::Building(Vector3* _position, Urho3D::Node* _boxNode) : Physical(_position, _boxNode, BUILDING) {
	hbMaxSize = 5.0;
}


Building::~Building() {
}

double Building::getHealthBarSize() {
	double healthBarSize = (5);
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}

int Building::getSubType() {
	return buildingType;
}

void Building::populate(db_building* dbBuilding) {
	minimalDistance = dbBuilding->minDist;
	buildingType = BuildingType(dbBuilding->type);
}

void Building::absorbAttack(double attackCoef) {
}

bool Building::isInGrandient() {
	return inGradient;
}

void Building::setInGradinet(bool _inGradient) {
	inGradient = _inGradient;
}
