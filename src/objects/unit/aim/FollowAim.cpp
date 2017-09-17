#include "FollowAim.h"
#include "TargetAim.h"
#include "objects/Physical.h"
#include "../Unit.h"


FollowAim::FollowAim(Physical* _physical) {
	physical = _physical;
	radius = 3;
}


FollowAim::~FollowAim() {
}

Urho3D::Vector3* FollowAim::getDirection(Unit* unit) {
	return new Vector3((*physical->getPosition()) - (*unit->getPosition()));
}

bool FollowAim::ifReach(Unit* unit) {
	float dist = ((*unit->getPosition()) - (*physical->getPosition())).LengthSquared();
	if (dist < radius * radius) {
		return true;
	}
	return false;
}
