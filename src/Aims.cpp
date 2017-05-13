#include "Aims.h"



Aims::Aims(int _references) {
	aims = new std::vector<Aim*>();
	aims->reserve(10);
	references = _references;
}

Aims::~Aims() {
	delete aims;
}

Aim* Aims::getAim(int index) {
	if (index >= aims->size()) { return nullptr; }
	return aims->at(index);
}

Urho3D::Vector3* Aims::getAimPos(int index) {
	if (index >= aims->size()) { return nullptr; }
	return (*aims)[index]->getPosition();
}

void Aims::clearAims() {
	if (references <= 0) {
		clear_vector(aims);
		references = 0;
	}
}

bool Aims::ifReach(Urho3D::Vector3* pedestrian, int index) {
	if (aims->size() == 0) { return false; }
	Aim* aim = getAim(index);
	if (aim == nullptr) { return false; }
	Urho3D::Vector3* pos = aim->getPosition();
	double distance = ((*pos) - (*pedestrian)).Length();
	if (distance <= aim->getRadius()) {
		return true;
	}
	return false;

}

void Aims::add(Urho3D::Vector3* pos) {
	pos->y_ = 0;
	aims->push_back(new Aim(pos));
}

bool Aims::check(int aimIndex) {
	if (aimIndex >= aims->size()) {
		reduce();
		return true;
	} else {
		return false;
	}
}

int Aims::getReferences() {
	return references;
}

void Aims::reduce() {
	--references;
}

void Aims::up() {
	++references;
}
