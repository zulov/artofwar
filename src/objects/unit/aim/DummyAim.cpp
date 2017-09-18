#include "DummyAim.h"


DummyAim::DummyAim() {
}


DummyAim::~DummyAim() {
}

Urho3D::Vector3* DummyAim::getDirection(Unit* unit) {
	return new Urho3D::Vector3();
}

bool DummyAim::ifReach(Unit* unit) {
	return true;
}

bool DummyAim::expired() {
	return false;
}
