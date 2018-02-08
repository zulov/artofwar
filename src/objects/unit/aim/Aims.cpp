#include "Aims.h"
#include "DummyAim.h"
#include "TargetAim.h"
#include "utils.h"


Aims::Aims() {
	current = 0;
}

Aims::~Aims() {
	clear_vector(aims);
}

Urho3D::Vector3* Aims::getDirection(Unit* unit) {
	if (current >= aims.size()) { return nullptr; }
	return aims[current]->getDirection(unit);
}

void Aims::clearAims() {
	for (int i = 0; i < aims.size(); ++i) {
		Aim* aim = aims[i];
		if (aim->expired()) {
			delete aim;
			aims[i] = new DummyAim();
		}
	}
	current = 0;
}

bool Aims::ifReach(Unit* unit) {
	if (current >= aims.size()) { return false; }
	Aim* aim = aims[current];
	if (aim == nullptr) { return false; }

	if (aim->ifReach(unit)) {
		++current;
		if (current >= aims.size()) {
			clear_vector(aims);
			current = 0;
			return true;
		}
	}
	return false;
}

bool Aims::hasAim() {
	return current < aims.size();
}

void Aims::add(Aim* aim) {
	aims.push_back(aim);
}
