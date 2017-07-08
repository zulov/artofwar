#include "Building.h"
#include "ObjectEnums.h"
#include "db_strcut.h"


Building::Building(Vector3* _position, Urho3D::Node* _boxNode) : Entity(_position, _boxNode) {

}


Building::~Building() {
}

double Building::getHealthBarSize() {
	double healthBarSize = (1);
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}


ObjectType Building::getType() {
	return BUILDING;
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
