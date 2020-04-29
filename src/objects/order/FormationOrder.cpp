#include "FormationOrder.h"
#include "Game.h"
#include "IndividualOrder.h"
#include "objects/unit/Unit.h"
#include "objects/order/enums/UnitAction.h"
#include "objects/order/enums/UnitActionType.h"
#include "simulation/formation/Formation.h"
#include "simulation/env/Environment.h"

FormationOrder::FormationOrder(Formation* formation, UnitActionType actionType, short action,
                               Urho3D::Vector2& vector, bool append):
	UnitOrder(actionType, action, append, vector), formation(formation) {
}

FormationOrder::FormationOrder(Formation* formation, UnitActionType actionType, short action,
                               Physical* toUse, bool append):
	UnitOrder(actionType, action, append, toUse), formation(formation) {
}


FormationOrder::~FormationOrder() = default;

bool FormationOrder::add() {
	//TODO to implement
	return false;
}

bool FormationOrder::expired() {
	return formation == nullptr
		|| formation->getSize() <= 0
		|| toUse != nullptr && !toUse->isAlive();
}

void FormationOrder::clean() {
	if (formation->getSize() <= 0) {
		formation = nullptr;
	}
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
		if (toUse == nullptr) {
			int a = 5;
		}
		auto posToUseOpt = toUse->getPosToUseWithIndex(static_cast<Unit*>(optLeader.value()));
		//TODO bug TuUSe moze nie istniec!!!!!!!!!
		if (posToUseOpt.has_value()) {
			auto postToUse = posToUseOpt.value();
			if (std::get<1>(postToUse) > distThreshold) {
				auto pos = std::get<0>(postToUse);
				optLeader.value()->action(UnitAction::FOLLOW,
				                          getFollowAim(optLeader.value()->getMainCell(), pos, toUse));
				formation->addOrder(
					new FormationOrder(formation, UnitActionType::ORDER, id, toUse, true));
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
		followAndAct(static_cast<Unit*>(optLeader.value())->getAttackRange());
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
		unit->action(static_cast<UnitAction>(id));
	}
}
