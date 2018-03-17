#include "Aims.h"
#include "DummyAim.h"
#include "TargetAim.h"
#include "utils.h"
#include <optional>


Aims::Aims() {
	current = 0;
}

Aims::~Aims() {
	clear_vector(aims);
}

std::optional<Urho3D::Vector2> Aims::getDirection(Unit* unit) {
	if (current >= aims.size()) { return std::nullopt; }
	return aims[current]->getDirection(unit);
}

void Aims::clearAimsIfExpired() {
	for (auto& aim : aims) {
		if (aim->expired()) {
			delete aim;
			aim = new DummyAim();
		}
	}
}

bool Aims::ifReach(Unit* unit) {
	if (current >= aims.size()) { return false; }
	Aim* aim = aims[current];
	if (aim == nullptr) { return false; }

	if (aim->ifReach(unit)) {
		++current;
		if (current >= aims.size()) {
			clear();
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

void Aims::clear() {
	clear_vector(aims);
	current = 0;
}
