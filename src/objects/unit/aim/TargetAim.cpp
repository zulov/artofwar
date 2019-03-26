#include "TargetAim.h"
#include "../Unit.h"
#include "Game.h"
#include "MathUtils.h"
#include "simulation/env/Environment.h"


TargetAim::TargetAim(std::vector<int>& _path) :
	path(_path), current(0) {
	currentTarget = Game::getEnvironment()->getCenter(path[current]);
}


TargetAim::~TargetAim() = default;

std::vector<Urho3D::Vector3> TargetAim::getDebugLines(Unit* unit) const {
	std::vector<Urho3D::Vector3> points;
	points.emplace_back(*unit->getPosition());
	for (short i = current; i < path.size(); ++i) {
		auto center = Game::getEnvironment()->getCenter(path[i]);
		points.emplace_back(center.x_ , unit->getPosition()->y_, center.y_);
	}

	return points;
}

Urho3D::Vector2 TargetAim::getDirection(Unit* unit) {
	return {
		currentTarget.x_ - unit->getPosition()->x_,
		currentTarget.y_ - unit->getPosition()->z_
	};
}

bool TargetAim::ifReach(Unit* unit) {
	if (path[current]== unit->getMainCell()) {
		++current;
		if (current >= path.size()) {
			return true;
		}
		currentTarget = Game::getEnvironment()->getCenter(path[current]);
	}
	return false;
}

bool TargetAim::expired() {
	return false;
}
