#include "FormationOrder.h"
#include "Game.h"
#include "objects/unit/Unit.h"
#include "objects/unit/ActionParameter.h"
#include "simulation/formation/Formation.h"
#include "simulation/env/Environment.h"
#include "IndividualOrder.h"
#include "consts.h"


FormationOrder::FormationOrder(Formation* formation, UnitOrder action, const Urho3D::Vector2& vector,
                               Physical* physical, bool append):
	FutureOrder(action, append, vector, physical), formation(formation) {
}

FormationOrder::~FormationOrder() = default;

bool FormationOrder::add() {
	//TODO to implement
	return false;
}

void FormationOrder::addCollectAim() {
	followAndAct();
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

void FormationOrder::followAndAct() {
	auto optLeader = formation->getLeader();
	if (optLeader.has_value()) {
		auto posOpt = toUse->getPosToUseWithIndex(static_cast<Unit*>(optLeader.value()));
		if (posOpt.has_value()) {
			auto postToUse = posOpt.value();
			auto dist = std::get<1>(postToUse);
			if (dist > 25) {
				//TODO hardcode
				auto pos = std::get<0>(postToUse);
				optLeader.value()->action(static_cast<char>(UnitOrder::FOLLOW),
				                          getFollowAim(optLeader.value()->getMainCell(),
				                                       pos, toUse));
				formation->addOrder(new FormationOrder(formation, action, {}, toUse, true)); //Dodanie celu po dojsciu
			} else {
				for (auto unit : formation->getUnits()) {
					unit->resetFormation();
					unit->addOrder(new IndividualOrder(unit, action, {}, toUse, false));
					//TODO to samo zrobic w innnych akcjach z atakiem
					//TOAttack jak nie ten to zaatakowac blizeszego
				}
				formation->remove();
			}

		}
	}
}

void FormationOrder::addAttackAim() {
	followAndAct();
}

void FormationOrder::addDefendAim() {
	simpleAction();
}


void FormationOrder::addDeadAim() {
	simpleAction();
}

void FormationOrder::simpleAction() {
	for (auto unit : formation->getUnits()) {
		unit->action(static_cast<char>(action), Consts::EMPTY_ACTION_PARAMETER);
	}
}
