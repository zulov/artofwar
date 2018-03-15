#include "TargetAim.h"
#include "../Unit.h"
#include "Game.h"
#include "simulation/env/Enviroment.h"


TargetAim::TargetAim(std::vector<int>& _path) {
	radiusSq = 3 * 3;
	path = _path;
	current = 0;
}


TargetAim::~TargetAim() = default;

Urho3D::Vector3* TargetAim::getDirection(Unit* unit) {
	Vector2 position = Game::get()->getEnviroment()->getCenter(path[current]);
	//TODO storzyc currentPosition i zmienaic przy przejsciu
	Vector3* dir = new Vector3(
	                           position.x_ - unit->getPosition()->x_,
	                           0,
	                           position.y_ - unit->getPosition()->z_
	                          );

	return dir;
}

bool TargetAim::ifReach(Unit* unit) {
	Vector2 position = Game::get()->getEnviroment()->getCenter(path[current]);
	float dist = Vector3(
	                     position.x_ - unit->getPosition()->x_,
	                     0,
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
