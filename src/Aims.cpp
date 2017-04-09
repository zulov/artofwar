#include "Aims.h"
#include "utils.h"


Aims::Aims() {
	aims = new std::vector<Urho3D::Vector3*>();
	aims->reserve(10);
	index = 0;
	radius = 2;
}


Aims::~Aims() {
	delete aims;
}

Urho3D::Vector3* Aims::getAim() {
	if (index >= aims->size()) { return nullptr; }
	return aims->at(index);
}

bool Aims::check(Urho3D::Vector3* pedestrian) {
	if (aims->size() == 0) { return false; }
	Urho3D::Vector3* aim = getAim();
	if (aim == nullptr) { return false; }
	double distance = ((*aim) - (*pedestrian)).Length();
	if (distance <= radius) {
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
	aims->push_back(new Urho3D::Vector3(*pos));
}
