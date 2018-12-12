#include "Aims.h"
#include "Game.h"
#include "TargetAim.h"
#include "commands/action/IndividualAction.h"
#include "objects/unit/Unit.h"
#include "objects/unit/aim/FutureAim.h"
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
	                              [](FutureAim fa) { return fa.expired(); }),
	               nextAims.end());
	if (current != nullptr && current->expired()) {
		removeCurrentAim();
	}
}

bool Aims::ifReach(Unit* unit) {
	if (current == nullptr && nextAims.empty()) { return false; } //TODO bug true??
	if (current) {
		if (current->ifReach(unit) && nextAims.empty()) {
			clear();
			return true;
		}
	} else if (!nextAims.empty()) {
		current = IndividualAction::createAim(*nextAims.begin());
		// if (nextAims[0].physical != nullptr) {
		// 	
		// 	Game::getActionList()->add(new IndividualAction(unit, nextAims[0].action,
		// 	                                                nextAims[0].physical, true));
		// } else {
		// 	Game::getActionList()->add(new IndividualAction(unit, nextAims[0].action,
		// 	                                                nextAims[0].vector, true));
		// }
		nextAims.erase(nextAims.begin());
	}

	return false;
}

void Aims::add(const FutureAim& aim, bool append) {
	if (!append) {
		clear();
		current = IndividualAction::createAim(aim);
	} else {
		nextAims.push_back(aim);
	}
}

void Aims::clear() {
	nextAims.clear();
	removeCurrentAim();
}

void Aims::removeCurrentAim() {
	delete current;
	current = nullptr;
}

std::vector<Urho3D::Vector3> Aims::getDebugLines(Unit* unit) const {
	return current->getDebugLines(unit);
}
