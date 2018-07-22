#include "FollowAim.h"
#include "../Unit.h"
#include "MathUtils.h"
#include "Game.h"
#include "simulation/env/Enviroment.h"


FollowAim::FollowAim(const Physical* _physical): physical(_physical) {
	radiusSq = 1 * 1;
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
	const auto pos = physical->getPosToFollow(unit->getPosition());

	return Urho3D::Vector2(pos.x_ - unit->getPosition()->x_,
	                       pos.y_ - unit->getPosition()->z_);
}

bool FollowAim::ifReach(Unit* unit) {
	auto posToFollow = physical->getPosToFollow(unit->getPosition());
	auto reach = sqDist(*unit->getPosition(), posToFollow) < radiusSq;
	if (reach) {
		unit->setIndexToInteract(Game::getEnviroment()->getIndex(posToFollow));//TODO bug moze kiedys powstac
	}
	return reach;
}

bool FollowAim::expired() {
	return !physical->isAlive();
}
