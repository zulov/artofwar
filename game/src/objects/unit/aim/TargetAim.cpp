#include "TargetAim.h"

#include <utility>
#include "../Unit.h"
#include "Game.h"
#include "math/MathUtils.h"
#include "env/Environment.h"


TargetAim::TargetAim(std::vector<int> _path) :
	path(std::move(_path)), current(0) {
	currentTarget = Game::getEnvironment()->getCenter(path[current]);
}

std::vector<Urho3D::Vector3> TargetAim::getDebugLines(Unit* unit) const {
	std::vector<Urho3D::Vector3> points;
	points.emplace_back(unit->getPosition());
	const auto env = Game::getEnvironment();

	for (short i = current; i < path.size(); ++i) {
		auto center = env->getPosWithHeightAt(path[i]);
		points.emplace_back(center);
	}

	return points;
}

Urho3D::Vector2 TargetAim::getDirection(Unit* unit) {
	return dirTo(unit->getPosition(), currentTarget);
}

bool TargetAim::ifReach(Unit* unit) {
	if (path[current] == unit->getMainGridIndex()) {
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
