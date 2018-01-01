#include "DummyAim.h"


DummyAim::DummyAim() = default;


DummyAim::~DummyAim() = default;

Urho3D::Vector3* DummyAim::getDirection(Unit* unit) {
	return new Urho3D::Vector3();
}

bool DummyAim::ifReach(Unit* unit) {
	return true;
}

bool DummyAim::expired() {
	return false;
}
