#include "ChargeAim.h"

ChargeAim::ChargeAim(Urho3D::Vector2* _direction): direction(*_direction), chargeEnergy(50) {
	direction.Normalize();
}


ChargeAim::~ChargeAim() = default;

std::vector<Urho3D::Vector3> ChargeAim::getDebugLines(Urho3D::Vector3* position) {
	std::vector<Urho3D::Vector3> points;

	points.emplace_back(0, 0.5, 0);
	points.emplace_back(direction.x_, 0.5, direction.y_);

	return points; //TODO performance std::move
}

Urho3D::Vector2 ChargeAim::getDirection(Unit* unit) {
	return direction;
}

bool ChargeAim::ifReach(Unit* unit) {
	return false;
}

bool ChargeAim::expired() {
	return false;
}
