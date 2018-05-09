#include "ChargeAim.h"
#include "Game.h"

ChargeAim::ChargeAim(Urho3D::Vector2* _direction): direction(*_direction), chargeEnergy(50) {
	direction.Normalize();
}


ChargeAim::~ChargeAim() = default;

Urho3D::Vector2 ChargeAim::getDirection(Unit* unit) {
	return direction;
}

bool ChargeAim::ifReach(Unit* unit) {
	return false;
}

bool ChargeAim::expired() {
	return false;
}
