#include "FormationOrder.h"
#include "Game.h"
#include "consts.h"
#include "IndividualOrder.h"
#include "objects/unit/Unit.h"
#include "objects/order/enums/UnitAction.h"
#include "simulation/formation/Formation.h"
#include "simulation/env/Environment.h"

FormationOrder::FormationOrder(Formation* formation, UnitActionType actionType, UnitAction action,
                               const Urho3D::Vector2& vector, Physical* toUse, bool append):
	UnitOrder(actionType, action, append, toUse, vector), formation(formation) {
}

FormationOrder::~FormationOrder() = default;

bool FormationOrder::add() {
	//TODO to implement
	return false;
}

bool FormationOrder::expired() {
	return formation == nullptr || formation->getSize() <= 0;
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
	if (opt.has_value()) {
		opt.value()->action(static_cast<char>(action), getTargetAim(opt.value()->getMainCell(), vector));
		formation->stopAllBesideLeader();
		Game::getEnvironment()->invalidateCache();
	}
}

void FormationOrder::addFollowAim() {
	auto opt = formation->getLeader();
	if (opt.has_value()) {
		formation->stopAllBesideLeader();
		auto posOpt = toUse->getPosToUseBy(static_cast<Unit*>(opt.value()));
		if (posOpt.has_value()) {
			opt.value()->action(static_cast<char>(action),
			                    getFollowAim(opt.value()->getMainCell(),
			                                 posOpt.value(), toUse));
		}
	}
}

void FormationOrder::addChargeAim() {
	for (auto unit : formation->getUnits()) {
		unit->action(static_cast<char>(action), getChargeAim(vector));
	}
}

void FormationOrder::followAndAct(float distThreshold) {
	auto optLeader = formation->getLeader();
	if (optLeader.has_value()) {
		auto posToUseOpt = toUse->getPosToUseWithIndex(static_cast<Unit*>(optLeader.value()));
		//TODO bug TuUSe moze nie istniec
		if (posToUseOpt.has_value()) {
			auto postToUse = posToUseOpt.value();
			if (std::get<1>(postToUse) > distThreshold) {
				auto pos = std::get<0>(postToUse);
				optLeader.value()->action(static_cast<char>(UnitAction::FOLLOW),
				                          getFollowAim(optLeader.value()->getMainCell(),
				                                       pos, toUse));
				formation->addOrder(
					new FormationOrder(formation, UnitActionType::ORDER, UnitAction(action), {}, toUse, true));
				//Dodanie celu po dojsciu
			} else {
				for (auto unit : formation->getUnits()) {
					unit->resetFormation();
					unit->addOrder(
						new IndividualOrder(unit, UnitActionType::ORDER, UnitAction(action), {}, toUse, false));
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
		unit->action(static_cast<char>(action), Consts::EMPTY_ACTION_PARAMETER);
	}
}
