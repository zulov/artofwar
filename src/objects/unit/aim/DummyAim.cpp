#include "DummyAim.h"


DummyAim::DummyAim() = default;


DummyAim::~DummyAim() = default;

Urho3D::Vector2 DummyAim::getDirection(Unit* unit) {
	return Urho3D::Vector2();
}

bool DummyAim::ifReach(Unit* unit) {
	return true;
}

bool DummyAim::expired() {
	return false;
}
