#include "FollowAim.h"
#include "../Unit.h"
#include "Game.h"
#include "MathUtils.h"
#include "TargetAim.h"
#include "simulation/env/Environment.h"


FollowAim::FollowAim(const Physical* physical, TargetAim* subTarget): physical(physical), subTarget(subTarget) {
	radiusSq = 1 * 1;
}

FollowAim::~FollowAim() = default;

std::vector<Urho3D::Vector3> FollowAim::getDebugLines(Unit* unit) const {
	return subTarget->getDebugLines(unit);
	auto position = unit->getPosition();
	std::vector<Urho3D::Vector3> points;
	auto center = physical->getPosToUseBy(unit);
	points.emplace_back(*position);
	points.emplace_back(center.x_, position->y_, center.y_);

	return points;
}

Urho3D::Vector2 FollowAim::getDirection(Unit* follower) {
	const auto pos = physical->getPosToUseBy(follower);

	return {
		pos.x_ - follower->getPosition()->x_,
		pos.y_ - follower->getPosition()->z_
	};
}

bool FollowAim::ifReach(Unit* unit) {
	//subTarget->ifReach(unit);
	auto posToFollow = physical->getPosToUseBy(unit);
	return sqDist(*unit->getPosition(), posToFollow) < radiusSq;
}

bool FollowAim::expired() {
	return !physical->isAlive();
}
