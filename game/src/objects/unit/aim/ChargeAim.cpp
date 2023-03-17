#include "ChargeAim.h"
#include "../Unit.h"

ChargeAim::ChargeAim(Urho3D::Vector2* _direction): direction(*_direction), chargeEnergy(50) {
	direction.Normalize();
}

std::vector<Urho3D::Vector3> ChargeAim::getDebugLines(Unit* unit) const {
	std::vector<Urho3D::Vector3> points;
	auto position = unit->getPosition();

	points.emplace_back(position);
	points.emplace_back(position.x_ + direction.x_ * 10, position.y_, position.z_ + direction.y_ * 10);

	return points;
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
