#include "Aims.h"
#include "DummyAim.h"
#include "TargetAim.h"
#include <optional>
#include <algorithm>
#include "Game.h"
#include "commands/CommandList.h"
#include "commands/action/IndividualAction.h"
#include "objects/unit/aim/FutureAim.h"
#include "objects/unit/Unit.h"


Aims::Aims() {
	current = nullptr;
}

Aims::~Aims() {
	delete current;
}

std::optional<Urho3D::Vector2> Aims::getDirection(Unit* unit) {
	return current->getDirection(unit);
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
	if (current == nullptr) { return false; }

	if (current->ifReach(unit)) {

		if (nextAims.empty()) {
			clear();
			return true;
		} else {
			if (nextAims[0].physical != nullptr) {

				Game::get()->getActionCommandList()->add(new IndividualAction(unit, //TODO a moze formation, skad to wiedziec :(
				                                                              nextAims[0].action,
				                                                              nextAims[0].physical,
				                                                              true));
			} else {
				Game::get()->getActionCommandList()->add(new IndividualAction(unit,
				                                                              nextAims[0].action,
				                                                              nextAims[0].vector,
				                                                              true));
			}
			nextAims.erase(nextAims.begin());
		}
	}
	return false;
}

bool Aims::hasAim() {
	return current != nullptr || !nextAims.empty();
}

void Aims::add(Aim* aim) {
	delete current;
	current = aim;
}

void Aims::add(FutureAim& aim) {
	nextAims.push_back(aim);
}

void Aims::clear() {
	nextAims.clear();
	current = nullptr;
}
