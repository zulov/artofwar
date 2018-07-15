#include "FollowAim.h"
#include "../Unit.h"
#include "MathUtils.h"


FollowAim::FollowAim(const Physical* _physical): physical(_physical) {
	radiusSq = 3 * 3;
}

FollowAim::~FollowAim() = default;

std::vector<Urho3D::Vector3> FollowAim::getDebugLines(Urho3D::Vector3* position) {
	std::vector<Urho3D::Vector3> points;
	auto center = physical->getPosToFollow(position);
	points.emplace_back(0, 0.5, 0);
	points.emplace_back(center.x_ - position->x_, 0.5, center.y_ - position->z_);

	return points; //TODO performance std::move
}

Urho3D::Vector2 FollowAim::getDirection(Unit* unit) {
	Vector2 pos = physical->getPosToFollow(unit->getPosition());

	return Vector2(pos.x_ - unit->getPosition()->x_,
	               pos.y_ - unit->getPosition()->z_);
}

bool FollowAim::ifReach(Unit* unit) {
	return sqDist(*unit->getPosition(),
	              physical->getPosToFollow(unit->getPosition())) < radiusSq;
}

bool FollowAim::expired() {
	return !physical->isAlive();
}
