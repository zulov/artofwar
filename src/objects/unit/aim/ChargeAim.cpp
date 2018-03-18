#include "ChargeAim.h"
#include "Game.h"
#include <Urho3D/Scene/Scene.h>


ChargeAim::ChargeAim(Urho3D::Vector2* _direction): direction(*_direction), distance(50) {
	direction.Normalize();
}


ChargeAim::~ChargeAim() = default;

Vector2 ChargeAim::getDirection(Unit* unit) {
	return direction;
}

bool ChargeAim::ifReach(Unit* unit) {
	return false;
}

bool ChargeAim::expired() {
	return false;
}
