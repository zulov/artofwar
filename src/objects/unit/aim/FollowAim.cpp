#include "FollowAim.h"
#include "objects/Physical.h"
#include "../Unit.h"


FollowAim::FollowAim(Physical* _physical) {
	physical = _physical;
	radiusSq = 3 * 3;
}

FollowAim::~FollowAim() = default;

Urho3D::Vector3* FollowAim::getDirection(Unit* unit) {
	return new Vector3((*physical->getPosition()) - (*unit->getPosition()));
}

bool FollowAim::ifReach(Unit* unit) {
	const float dist = ((*unit->getPosition()) - (*physical->getPosition())).LengthSquared();
	return dist < radiusSq;
}

bool FollowAim::expired() {
	return !physical->isAlive();
}
