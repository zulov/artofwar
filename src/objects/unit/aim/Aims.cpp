#include "Aims.h"
#include "defines.h"


Aims::Aims() {
	references = 0;
}

Aims::~Aims() {
}

Aim* Aims::getAim(short index) {
	if (index >= aims.size()) { return nullptr; }
	return aims[index];
}

Urho3D::Vector3* Aims::getAimPos(short index) {
	if (index >= aims.size()) { return nullptr; }
	return aims[index]->getPosition();
}

void Aims::clearAims() {
	if (references <= 0) {
		clear_vector(aims);
		references = 0;
	}
}

bool Aims::ifReach(Urho3D::Vector3* pedestrian, int index) {
	if (aims.size() == 0) { return false; }
	Aim* aim = getAim(index);
	if (aim == nullptr) { return false; }
	Urho3D::Vector3* pos = aim->getPosition();
	double distance = ((*pos) - (*pedestrian)).LengthSquared();
	if (distance <= aim->getRadius()*aim->getRadius()) {
		return true;
	}
	return false;

}

void Aims::add(Urho3D::Vector3* pos) {
	pos->y_ = 0;
	aims.push_back(new Aim(pos));
}

bool Aims::check(int aimIndex) {
	if (aimIndex >= aims.size()) {
		reduce();
		return true;
	}
	return false;

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
