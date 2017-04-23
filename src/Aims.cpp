#include "Aims.h"
#include "utils.h"
#include <Urho3D/Graphics/StaticModel.h>
#include "Game.h"


Aims::Aims() {
	aims = new std::vector<Aim*>();
	aims->reserve(10);
	index = 0;
}


Aims::~Aims() {
	delete aims;
}

Aim* Aims::getAim() {
	if (index >= aims->size()) { return nullptr; }
	return aims->at(index);
}

Vector3* Aims::getAimPos() {
	if (index >= aims->size()) { return nullptr; }
	return (*aims)[index]->getPosition();
}

bool Aims::check(Urho3D::Vector3* pedestrian) {
	if (aims->size() == 0) { return false; }
	Aim* aim = getAim();
	if (aim == nullptr) { return false; }
	Vector3* pos = aim->getPosition();
	double distance = ((*pos) - (*pedestrian)).Length();
	if (distance <= aim->getRadius()) {
		index++;
	}
	if (index >= aims->size()) {
		index = 0;
		clear_vector(aims);
		return true;
	} else {
		return false;
	}
}

void Aims::add(Entity* entity) {
	Urho3D::Vector3* pos = entity->getPosition();

	pos->y_ = 0;
	aims->push_back(new Aim(pos));

}
