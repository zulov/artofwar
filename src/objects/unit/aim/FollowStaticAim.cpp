#include "FollowStaticAim.h"
#include "MathUtils.h"
#include "objects/Static.h"
#include "objects/unit/Unit.h"


FollowStaticAim::FollowStaticAim(Static* _staticObj): staticObj(_staticObj) {
	radiusSq = 3 * 3;
}


FollowStaticAim::~FollowStaticAim() = default;

Urho3D::Vector2 FollowStaticAim::getDirection(Unit* unit) {
	const Vector3 closestCell = staticObj->getClosestCellPos(unit->getPosition());
	return Vector2(
	               closestCell.x_ - unit->getPosition()->x_,
	               closestCell.z_ - unit->getPosition()->z_
	              );
}

bool FollowStaticAim::ifReach(Unit* unit) {
	const Vector3 closestCell = staticObj->getClosestCellPos(unit->getPosition());
	const float distance = sqDist(*unit->getPosition(), closestCell);
	return distance < radiusSq;
}

bool FollowStaticAim::expired() {
	return !staticObj->isAlive();
}
