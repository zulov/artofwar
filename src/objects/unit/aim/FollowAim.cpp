#include "FollowAim.h"
#include "../Unit.h"
#include "MathUtils.h"
#include "TargetAim.h"
#include "simulation/env/Environment.h"


FollowAim::FollowAim(const Physical* physical, TargetAim* subTarget): physical(physical), subTarget(subTarget) {
	radiusSq = 1 * 1;
}

FollowAim::~FollowAim() = default;

std::vector<Urho3D::Vector3> FollowAim::getDebugLines(Unit* follower) const {
	return subTarget->getDebugLines(follower);
	auto position = follower->getPosition();
	std::vector<Urho3D::Vector3> points;
	auto center = physical->getPosToUseBy(follower);
	points.emplace_back(*position);
	//points.emplace_back(center.x_, position->y_, center.y_);

	return points;
}

Urho3D::Vector2 FollowAim::getDirection(Unit* follower) {
	const auto opt = physical->getPosToUseBy(follower);
	if (opt.has_value()) {
		return dirTo(follower->getPosition(), opt.value());
	}
	return {};

}

bool FollowAim::ifReach(Unit* follower) {
	//subTarget->ifReach(unit);
	auto opt = physical->getPosToUseBy(follower);
	if (opt.has_value()) {
		sqDist(*follower->getPosition(), opt.value()) < radiusSq;
	}
	return true;
}

bool FollowAim::expired() {
	return !physical->isAlive();
}
