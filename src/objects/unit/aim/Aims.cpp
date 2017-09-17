#include "Aims.h"
#include "TargetAim.h"


Aims::Aims() {
	references = 0;
}

Aims::~Aims() {
}

Urho3D::Vector3* Aims::getDirection(Unit* unit, short index) {
	if (index >= aims.size()) { return nullptr; }
	return aims[index]->getDirection(unit);
}

void Aims::clearAims() {
	if (references <= 0) {
		clear_vector(aims);
		references = 0;
	}
}

bool Aims::ifReach(Unit* unit, int index) {
	if (index >= aims.size()) { return false; }
	Aim* aim = aims[index];
	if (aim == nullptr) { return false; }
	return aim->ifReach(unit);
}

void Aims::add(Aim *aim) {
	aims.push_back(aim);
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
