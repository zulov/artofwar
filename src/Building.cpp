#include "Building.h"
#include "ObjectEnums.h"


Building::Building(Vector3 _position, Urho3D::Node* _boxNode) : Entity(_position, _boxNode) {

}


Building::~Building() {}


int Building::getType() {
	return BUILDING;
}