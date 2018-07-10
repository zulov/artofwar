#include "TargetAim.h"
#include "../Unit.h"
#include "Game.h"
#include "MathUtils.h"
#include "simulation/env/Enviroment.h"


TargetAim::TargetAim(std::vector<int>& _path) :
	radiusSq(3 * 3), path(_path), current(0) {
	currentTarget = Game::getEnviroment()->getCenter(path[current]);
}


TargetAim::~TargetAim() = default;

std::vector<Urho3D::Vector3> TargetAim::getDebugLines(Urho3D::Vector3* position) {
	std::vector<Urho3D::Vector3> points;
	points.emplace_back(0, 0.5, 0);
	for (short i = current; i < path.size(); ++i) {
		auto center = Game::getEnviroment()->getCenter(path[i]);
		points.emplace_back(center.x_ - position->x_,0.5, center.y_ - position->z_);
	}
	return points; //TODO performance std::move
}

Urho3D::Vector2 TargetAim::getDirection(Unit* unit) {
	return Vector2(currentTarget.x_ - unit->getPosition()->x_,
	               currentTarget.y_ - unit->getPosition()->z_);
}

bool TargetAim::ifReach(Unit* unit) {
	if (sqDist(currentTarget, *unit->getPosition()) < radiusSq) {
		++current;
		if (current >= path.size()) {
			return true;
		}
		currentTarget = Game::getEnviroment()->getCenter(path[current]);
	}
	return false;
}

bool TargetAim::expired() {
	return false;
}
