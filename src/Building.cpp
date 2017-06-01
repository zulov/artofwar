#include "Building.h"
#include "ObjectEnums.h"
#include "db_strcut.h"


Building::Building(Vector3* _position, Urho3D::Node* _boxNode, Font* _font) : Entity(_position, _boxNode, _font) {
	
}


Building::~Building() {
}


int Building::getType() {
	return BUILDING;
}

void Building::populate(db_building* dbBuilding) {
	minimalDistance = dbBuilding->minDist;
}

void Building::absorbAttack(double attackCoef) {
}
