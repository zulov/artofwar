#include "Aims.h"
#include "TargetAim.h"
#include "../order/OrderUtils.h"


Aims::Aims(): current(nullptr) {
}

Aims::~Aims() {
	delete current;
	clear_vector(nextAims);
}

std::optional<Urho3D::Vector2> Aims::getDirection(Unit* unit) const {
	if (current) {
		return current->getDirection(unit);
	}
	return {};
}

void Aims::clearExpired() {
	removeExpired(nextAims);
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
		auto toExecute = nextAims[0];
		nextAims.erase(nextAims.begin());
		toExecute->execute();
		delete toExecute;
	}

	return false;
}

void Aims::add(UnitOrder* order) {
	if (!order->getAppend()) {
		clear();
	}
	nextAims.push_back(order);
}

void Aims::clear() {
	clear_vector(nextAims);
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
