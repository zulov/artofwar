#include "Aims.h"
#include "DummyAim.h"
#include "Game.h"
#include "TargetAim.h"
#include "commands/CommandList.h"
#include "commands/action/IndividualAction.h"
#include "objects/unit/Unit.h"
#include "objects/unit/aim/FutureAim.h"
#include <algorithm>
#include <optional>


Aims::Aims() {
	current = nullptr;
}

Aims::~Aims() {
	delete current;
}

std::optional<Urho3D::Vector2> Aims::getDirection(Unit* unit) const {
	if (current) {
		return current->getDirection(unit);
	}
	return std::nullopt;
}

void Aims::clearExpired() {
	nextAims.erase(std::remove_if(nextAims.begin(),
	                              nextAims.end(),
	                              [](FutureAim fa) { return fa.expired(); }),
	               nextAims.end());
	if (current != nullptr && current->expired()) {
		delete current;
		current = nullptr;
	}
}

bool Aims::ifReach(Unit* unit) {
	if (current == nullptr && nextAims.empty()) { return false; }
	if (current) {
		if (current->ifReach(unit) && nextAims.empty()) {
			clear();
			return true;
		}
	} else if (!nextAims.empty()) {
		if (nextAims[0].physical != nullptr) {
			Game::getActionCommandList()->add(new IndividualAction(unit, nextAims[0].action,
			                                                              nextAims[0].physical, true));
		} else {
			Game::getActionCommandList()->add(new IndividualAction(unit, nextAims[0].action,
			                                                              nextAims[0].vector, true));
		}
		nextAims.erase(nextAims.begin());
	}

	return false;
}

void Aims::add(Aim* aim) {
	delete current;
	current = aim;
}

void Aims::add(const FutureAim& aim) {
	nextAims.push_back(aim);
}

void Aims::clear() {
	nextAims.clear();
	current = nullptr;
}

void Aims::removeCurrentAim() {
	delete current;
	current = nullptr;
}
