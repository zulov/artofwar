#include "ChargeAim.h"
#include "../Unit.h"
#include <Urho3D/Scene/Node.h>

ChargeAim::ChargeAim(Urho3D::Vector2* _direction): direction(*_direction), chargeEnergy(50) {
	direction.Normalize();
}

std::vector<Urho3D::Vector3> ChargeAim::getDebugLines(Unit* unit) const {
	std::vector<Urho3D::Vector3> points;
	auto nodePos = unit->getNode()->GetPosition();

	points.emplace_back(nodePos);
	points.emplace_back(nodePos.x_ + direction.x_ * 10, nodePos.y_, nodePos.z_ + direction.y_ * 10);

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
