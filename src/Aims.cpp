#include "Aims.h"


Aims::Aims() {
	aims = new std::vector<Urho3D::Vector3*>();
	aims->reserve(10);
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
		index = aims->size() - 1;
		return true;
	} else {
		return false;
	}
}
