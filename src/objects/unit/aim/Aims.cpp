#include "Aims.h"
#include "Game.h"
#include "TargetAim.h"
#include "objects/unit/Unit.h"
#include "objects/unit/aim/order/FutureOrder.h"
#include <algorithm>
#include <optional>


Aims::Aims(): current(nullptr) {
}

Aims::~Aims() {
	delete current;
}

std::optional<Urho3D::Vector2> Aims::getDirection(Unit* unit) const {
	if (current) {
		return current->getDirection(unit);
	}
	return {};
}

void Aims::clearExpired() {
	nextAims.erase(std::remove_if(nextAims.begin(), nextAims.end(),
	                              [](FutureOrder* fa) { return fa == nullptr || fa->expired(); }),
	               nextAims.end());
	if (current != nullptr && current->expired()) {
		removeCurrentAim();
	}
}

bool Aims::ifReach(Unit* unit) {
	if (current) {
		if (current->ifReach(unit)) {
			removeCurrentAim();
			return nextAims.empty();
		}
	} else if (!nextAims.empty()) {
		nextAims[0]->execute();
		delete nextAims[0];
		nextAims.erase(nextAims.begin());
		return true;
	}

	return false;
}

void Aims::add(FutureOrder* aim, bool append) {
	if (!append) {
		clear();
	}
	nextAims.push_back(aim);
}

void Aims::clear() {
	clear_vector(&nextAims);
	removeCurrentAim();
}

void Aims::removeCurrentAim() {
	delete current;
	current = nullptr;
}

std::vector<Urho3D::Vector3> Aims::getDebugLines(Unit* unit) const {
	return current->getDebugLines(unit);
}

void Aims::set(Aim* aim) {
	current = aim;
}
