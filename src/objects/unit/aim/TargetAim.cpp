#include "TargetAim.h"
#include "Game.h"
#include "../Unit.h"
#include "simulation/env/Enviroment.h"


TargetAim::TargetAim(Urho3D::Vector3* _position) {
	radiusSq = 3*3;
	path.reserve(100);
	current = 0;
}


TargetAim::~TargetAim() {
}

Urho3D::Vector3* TargetAim::getDirection(Unit* unit) {
	Vector3 position = Game::get()->getEnviroment()->getCenter(path[current]);
	//TODO storzyc currentPosition i zmienaic przy przejsciu
	return new Vector3((position) - (*unit->getPosition()));
}

bool TargetAim::ifReach(Unit* unit) {
	Vector3 position = Game::get()->getEnviroment()->getCenter(path[current]);
	//TODO storzyc currentPosition i zmienaic przy przejsciu
	float dist = ((*unit->getPosition()) - position).LengthSquared();
	bool reach = dist < radiusSq;
	if (reach) {
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
