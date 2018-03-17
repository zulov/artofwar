#include "ChargeAim.h"
#include "Game.h"
#include <Urho3D/Scene/Scene.h>


ChargeAim::ChargeAim(Urho3D::Vector3* _direction): direction(*_direction) {
	direction.y_ = 0;
	direction.Normalize();
	distance = 50;
}


ChargeAim::~ChargeAim() = default;

Vector2 ChargeAim::getDirection(Unit* unit) {
	return Urho3D::Vector2(
	                       direction.x_,
	                       direction.z_
	                      );
}

bool ChargeAim::ifReach(Unit* unit) {
	return false;
}

bool ChargeAim::expired() {
	return false;
}
