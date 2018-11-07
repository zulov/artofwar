#include "FormationAction.h"
#include "Game.h"
#include "objects/unit/Unit.h"
#include "objects/unit/state/StateManager.h"
#include "simulation/env/Environment.h"
#include "simulation/formation/Formation.h"

FormationAction::FormationAction(Formation* formation, UnitOrder action, const Physical* physical,
                                 Urho3D::Vector2* vector,
                                 bool append): ActionCommand(action, physical, vector, append), formation(formation) {
}

FormationAction::~FormationAction() = default;

void FormationAction::addTargetAim(Urho3D::Vector2* to, bool append) {
	auto opt = formation->getLeader();
	if (opt.has_value()) {

		opt.value()->action(static_cast<char>(action), getTargetAim(opt.value()->getMainCell(), *to));
		for (auto unit : formation->getUnits()) {
			if (unit != opt.value()) {
				StateManager::changeState(unit, UnitState::STOP);
			}
		}
		Game::getEnvironment()->invalidateCache();
	}
}

void FormationAction::addChargeAim(Urho3D::Vector2* charge, bool append) {
	for (auto unit : formation->getUnits()) {
		unit->action(static_cast<char>(action), getChargeAim(charge));
	}
}

void FormationAction::addFollowAim(const Physical* toFollow, bool append) {
	auto opt = formation->getLeader();
	if (opt.has_value()) {
		for (auto unit : formation->getUnits()) {
			if (unit != opt.value()) {
				StateManager::changeState(unit, UnitState::STOP);
			}
		}
		auto posOpt = toFollow->getPosToUseBy(static_cast<Unit*>(opt.value()));
		if (posOpt.has_value()) {
			opt.value()->action(static_cast<char>(action),
			                    getFollowAim(opt.value()->getMainCell(),
			                                 posOpt.value(), toFollow));
		}
	}
}

void FormationAction::addAttackAim(const Physical* toAttack, bool append) {
	auto opt = formation->getLeader();
	if (opt.has_value()) {
		auto posOpt = toAttack->getPosToUseWithIndex(static_cast<Unit*>(opt.value()));
		if (posOpt.has_value()) {
			auto pos = posOpt.value();
			opt.value()->action(static_cast<char>(action),
			                    getFollowAim(opt.value()->getMainCell(),
			                                 pos, toAttack));
		}
	}
}

void FormationAction::addDeadAim() {
	for (auto unit : formation->getUnits()) {
		unit->action(static_cast<char>(action), ActionParameter());
	}
}

void FormationAction::addDefendAim() {
	for (auto unit : formation->getUnits()) {
		unit->action(static_cast<char>(action), ActionParameter());
	}
}
