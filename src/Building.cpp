#include "Building.h"
#include "ObjectEnums.h"


Building::Building(Vector3* _position, Urho3D::Node* _boxNode, Font* _font) : Entity(_position, _boxNode, _font) {
	minimalDistance = 4;
}


Building::~Building() {
}


int Building::getType() {
	return BUILDING;
}
