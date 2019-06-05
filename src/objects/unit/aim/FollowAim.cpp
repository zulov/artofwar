#include "FollowAim.h"
#include "../Unit.h"
#include "MathUtils.h"
#include "TargetAim.h"
#include "simulation/env/Environment.h"


FollowAim::FollowAim(Physical* physical, TargetAim* subTarget): physical(physical), subTarget(subTarget) {
	radiusSq = 1 * 1;
}

FollowAim::~FollowAim() {
	delete subTarget;
}

std::vector<Urho3D::Vector3> FollowAim::getDebugLines(Unit* follower) const {
	if (subTarget) {
		return subTarget->getDebugLines(follower);
	}
	auto position = follower->getPosition();
	std::vector<Urho3D::Vector3> points;
	auto optPos = physical->getPosToUseBy(follower);
	if (optPos.has_value()) {
		auto pos = optPos.value();
		points.emplace_back(position);
		points.emplace_back(pos.x_, position.y_, pos.y_);
	}

	return points;
}

Urho3D::Vector2 FollowAim::getDirection(Unit* follower) {
	if (subTarget) {
		return subTarget->getDirection(follower);
	}
	const auto opt = physical->getPosToUseBy(follower);
	if (opt.has_value()) {
		auto val = opt.value();
		return dirTo(follower->getPosition(), val);
	}
	return {};
}

bool FollowAim::ifReach(Unit* follower) {
	if (subTarget) {
		return subTarget->ifReach(follower);
	}
	auto opt = physical->getPosToUseBy(follower);
	if (opt.has_value()) {
		return sqDist(follower->getPosition(), opt.value()) < radiusSq;
	}
	return true;
}

bool FollowAim::expired() {
	return !physical->isAlive();
}
