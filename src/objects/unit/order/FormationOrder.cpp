#include "FormationOrder.h"
#include "Game.h"
#include "IndividualOrder.h"
#include "objects/unit/Unit.h"
#include "enums/UnitAction.h"
#include "simulation/formation/Formation.h"
#include "simulation/env/Environment.h"
#include "objects/unit/ActionParameter.h"

FormationOrder::FormationOrder(Formation* formation, short action,
                               Urho3D::Vector2& vector, bool append):
	UnitOrder(action, append, vector), formation(formation) {
}

FormationOrder::FormationOrder(Formation* formation, short action,
                               Physical* toUse, bool append):
	UnitOrder(action, append, toUse), formation(formation) {
}

bool FormationOrder::add() {
	//TODO to implement
	return false;
}

bool FormationOrder::expired() {
	return formation == nullptr
		|| formation->getSize() <= 0
		|| toUse != nullptr && !toUse->isAlive();
}

short FormationOrder::getSize() const {
	return formation->getSize();
}

void FormationOrder::addCollectAim() {
	followAndAct(9);
}

void FormationOrder::addTargetAim() {
	auto opt = formation->getLeader();
	if (opt.has_value() && vector) {
		opt.value()->action(static_cast<UnitAction>(id), getTargetAim(opt.value()->getMainCell(), *vector));
		formation->stopAllBesideLeader();
		Game::getEnvironment()->invalidateCache();
	}
}

void FormationOrder::addFollowAim() {
	auto opt = formation->getLeader();
	if (opt.has_value()) {
		formation->stopAllBesideLeader();
		auto posOpt = toUse->getPosToUseBy(opt.value());
		if (posOpt.has_value()) {
			opt.value()->action(static_cast<UnitAction>(id),
			                    getFollowAim(opt.value()->getMainCell(),
			                                 posOpt.value(), toUse));
		}
	}
}

void FormationOrder::addChargeAim() {
	for (auto unit : formation->getUnits()) {
		unit->action(static_cast<UnitAction>(id), getChargeAim(*vector));
	}
}

void FormationOrder::followAndAct(float distThreshold) {
	auto optLeader = formation->getLeader();
	if (optLeader.has_value()) {
		auto posToUseOpt = toUse->getPosToUseWithIndex(optLeader.value());
		if (posToUseOpt.has_value()) {
			auto postToUse = posToUseOpt.value();
			if (std::get<1>(postToUse) > distThreshold) {
				auto pos = std::get<0>(postToUse);
				optLeader.value()->action(UnitAction::FOLLOW,
				                          getFollowAim(optLeader.value()->getMainCell(), pos, toUse));
				formation->addOrder(
					new FormationOrder(formation, id, toUse, true));
				//Dodanie celu po dojsciu
			} else {
				for (auto unit : formation->getUnits()) {
					unit->resetFormation();
					unit->addOrder(new IndividualOrder(unit, UnitAction(id), toUse, false));
					//TODO to samo zrobic w innnych akcjach z atakiem
					//TOAttack jak nie ten to zaatakowac blizeszego
				}
				formation->remove();
			}
		}
	}
}

void FormationOrder::addAttackAim() {
	auto optLeader = formation->getLeader();
	if (optLeader.has_value()) {
		followAndAct(optLeader.value()->getAttackRange());
	}
}

void FormationOrder::addDefendAim() {
	simpleAction();
}


void FormationOrder::addDeadAim() {
	simpleAction();
}

void FormationOrder::addStopAim() {
	simpleAction();
}

void FormationOrder::simpleAction() const {
	for (auto unit : formation->getUnits()) {
		unit->resetFormation();
		unit->action(static_cast<UnitAction>(id));
	}
	formation->remove();
}
