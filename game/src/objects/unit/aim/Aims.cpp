#include "Aims.h"
#include "TargetAim.h"
#include "../order/OrderUtils.h"
#include "objects/unit/order/IndividualOrder.h"
#include "utils/DeleteUtils.h"


Aims::Aims(): current(nullptr) {
}

Aims::~Aims() {
	delete current;
	clear_vector(nextAims);
}

Urho3D::Vector2 Aims::getDirection(Unit* unit) const {
	return current->getDirection(unit);
}

void Aims::clearExpired() {
	removeExpired(nextAims);
	if (current != nullptr && current->expired()) {
		removeCurrentAim();
	}
}

bool Aims::process(Unit* unit) {//TODO bug to chyba źle zwraca nie wiem jaki był tego cel
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

void Aims::add(IndividualOrder* order) {
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
