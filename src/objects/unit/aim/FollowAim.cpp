#include "FollowAim.h"
#include "../Unit.h"
#include "MathUtils.h"


FollowAim::FollowAim(const Physical* _physical): physical(_physical) {
	radiusSq = 3 * 3;
}

FollowAim::~FollowAim() = default;

Urho3D::Vector2 FollowAim::getDirection(Unit* unit) {
	return Vector2(
	               physical->getPosition()->x_ - unit->getPosition()->x_,
	               physical->getPosition()->z_ - unit->getPosition()->z_
	              );
}

bool FollowAim::ifReach(Unit* unit) {
	const float distance = sqDist(unit, physical);
	return distance < radiusSq;
}

bool FollowAim::expired() {
	return !physical->isAlive();
}
