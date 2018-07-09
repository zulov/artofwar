#include "TargetAim.h"
#include "../Unit.h"
#include "Game.h"
#include "MathUtils.h"
#include "simulation/env/Enviroment.h"


TargetAim::TargetAim(std::vector<int>& _path) :
	radiusSq(3 * 3), path(_path), current(0) {
}


TargetAim::~TargetAim() = default;

std::vector<Urho3D::Vector3> TargetAim::getDebugLines(Urho3D::Vector3* position) {
	std::vector<Urho3D::Vector3> points;
	points.emplace_back(0, 2, 0);
	for (short i = current; i < path.size(); ++i) {
		auto center = Game::getEnviroment()->getCenter(path[i]);
		points.emplace_back(center.x_ - position->x_, 2, center.y_ - position->z_);
	}
	return points;//TODO performance std::move
}

Urho3D::Vector2 TargetAim::getDirection(Unit* unit) {
	Vector2 position = Game::getEnviroment()->getCenter(path[current]);
	//TODO storzyc currentPosition i zmienaic przy przejsciu
	return Vector2(
	               position.x_ - unit->getPosition()->x_,
	               position.y_ - unit->getPosition()->z_
	              );
}

bool TargetAim::ifReach(Unit* unit) {
	Vector2 position = Game::getEnviroment()->getCenter(path[current]);
	float dist = sqDist(position, *unit->getPosition());

	if (dist < radiusSq) {
		++current;
		if (current >= path.size()) {
			return true;
		}
	}
	return false;
}

bool TargetAim::expired() {
	return false;
}
