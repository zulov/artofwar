#include "TargetAim.h"
#include "../Unit.h"
#include "Game.h"
#include "simulation/env/Enviroment.h"


TargetAim::TargetAim(std::vector<int>& _path) :
	radiusSq(3 * 3), path(_path), current(0) {
}


TargetAim::~TargetAim() = default;

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
	float dist = Vector2(
	                     position.x_ - unit->getPosition()->x_,
	                     position.y_ - unit->getPosition()->z_
	                    ).LengthSquared();
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
