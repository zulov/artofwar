#include "FormationOrder.h"
#include "Game.h"
#include "objects/unit/state/UnitState.h"
#include "objects/unit/Unit.h"
#include "objects/unit/state/StateManager.h"
#include "simulation/formation/Formation.h"
#include "simulation/env/Environment.h"


FormationOrder::FormationOrder(Formation* formation, UnitOrder action, const Urho3D::Vector2& vector,
                               const Physical* physical):
	FutureOrder(action, vector, physical), formation(formation) {
}


FormationOrder::~FormationOrder() = default;

bool FormationOrder::add(bool append) {
	//TODO to implement
	return false;
}

void FormationOrder::addTargetAim() {
	auto opt = formation->getLeader();
	if (opt.has_value()) {
		opt.value()->action(static_cast<char>(action), getTargetAim(opt.value()->getMainCell(), vector));
		for (auto unit : formation->getUnits()) {
			if (unit != opt.value()) {
				StateManager::changeState(unit, UnitState::STOP);
			}
		}
		Game::getEnvironment()->invalidateCache();
	}
}

void FormationOrder::addFollowAim() {
}

void FormationOrder::addChargeAim() {
}

void FormationOrder::addAttackAim() {
}

void FormationOrder::addDefendAim() {
}

void FormationOrder::addDeadAim() {
}

//
// void FormationAction::addChargeAim(Urho3D::Vector2* charge, bool append) {
// 	for (auto unit : formation->getUnits()) {
// 		unit->action(static_cast<char>(action), getChargeAim(charge));
// 	}
// }
//
// void FormationAction::addFollowAim(const Physical* toFollow, bool append) {
// 	auto opt = formation->getLeader();
// 	if (opt.has_value()) {
// 		for (auto unit : formation->getUnits()) {
// 			if (unit != opt.value()) {
// 				StateManager::changeState(unit, UnitState::STOP);
// 			}
// 		}
// 		auto posOpt = toFollow->getPosToUseBy(static_cast<Unit*>(opt.value()));
// 		if (posOpt.has_value()) {
// 			opt.value()->action(static_cast<char>(action),
// 			                    getFollowAim(opt.value()->getMainCell(),
// 			                                 posOpt.value(), toFollow));
// 		}
// 	}
// }
//
// void FormationAction::addAttackAim(const Physical* toAttack, bool append) {
// 	auto opt = formation->getLeader();
// 	if (opt.has_value()) {
// 		auto posOpt = toAttack->getPosToUseWithIndex(static_cast<Unit*>(opt.value()));
// 		if (posOpt.has_value()) {
// 			auto res = posOpt.value();
// 			auto dist = std::get<1>(res);
// 			if (dist > 25) {
// 				//TODO hardcode
// 				auto pos = std::get<0>(res);
// 				opt.value()->action(static_cast<char>(action),
// 				                    getFollowAim(opt.value()->getMainCell(),
// 				                                 pos, toAttack));
// 				formation->addAim({}, toAttack, action, true); //Dodanie celu po dojsciu
// 			} else {
// 				for (auto unit : formation->getUnits()) {
// 					unit->resetFormation();
// 					unit->addAim(FutureAim(toAttack, UnitOrder::ATTACK), false);
// 					//TODO to samo zrobic w innnych akcjach z atakiem
// 					//TOAttack jak nie ten to zaatakowac blizeszego
// 				}
// 				formation->remove();
// 			}
//
// 		}
// 	}
// }
//
// void FormationAction::addDeadAim() {
// 	for (auto unit : formation->getUnits()) {
// 		unit->action(static_cast<char>(action), ActionParameter());
// 	}
// }
//
// void FormationAction::addDefendAim() {
// 	for (auto unit : formation->getUnits()) {
// 		unit->action(static_cast<char>(action), ActionParameter());
// 	}
// }