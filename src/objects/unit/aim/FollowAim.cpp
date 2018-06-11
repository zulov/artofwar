#include "FollowAim.h"
#include "../Unit.h"
#include "MathUtils.h"


FollowAim::FollowAim(const Physical* _physical): physical(_physical) {
	radiusSq = 3 * 3;
}

FollowAim::~FollowAim() = default;

Urho3D::Vector2 FollowAim::getDirection(Unit* unit) {
	Vector2 pos = physical->getPosToFollow(unit->getPosition());

	return Vector2(
	               pos.x_ - unit->getPosition()->x_,
	               pos.y_ - unit->getPosition()->z_
	              );
}

bool FollowAim::ifReach(Unit* unit) {
	Vector2 pos = physical->getPosToFollow(unit->getPosition());
	const float distance = sqDist(*unit->getPosition(), pos);
	return distance < radiusSq;
}

bool FollowAim::expired() {
	return !physical->isAlive();
}
