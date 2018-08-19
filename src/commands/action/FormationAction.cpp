#include "FormationAction.h"
#include "Game.h"
#include "objects/unit/Unit.h"
#include "objects/unit/state/StateManager.h"
#include "simulation/env/Enviroment.h"


FormationAction::FormationAction(Formation* formation, UnitOrder action, const Physical* physical,
                                 Urho3D::Vector2* vector,
                                 bool append): ActionCommand(action, physical, vector, append) {
	this->formation = formation;
}

FormationAction::~FormationAction() = default;

void FormationAction::addTargetAim(Urho3D::Vector2* to, bool append) {
	auto opt = formation->getLeader();
	if (opt.has_value()) {
		ActionParameter parameter = getTargetAim(opt.value()->getMainCell(), *to, append);

		const auto id = static_cast<char>(action);
		opt.value()->action(id, parameter);
		for (auto unit : formation->getUnits()) {
			if (unit != opt.value()) {
				StateManager::changeState(unit, UnitState::STOP);
			}
		}
		Game::getEnviroment()->invalidateCache();
	}
}

void FormationAction::addChargeAim(Urho3D::Vector2* charge, bool append) {
	for (auto unit : formation->getUnits()) {
		const char id = static_cast<char>(action);
		ActionParameter parameter = getChargeAim(charge, append);
		unit->action(id, parameter);
	}
}

void FormationAction::addFollowAim(const Physical* toFollow, bool append) {
	auto opt = formation->getLeader();
	if (opt.has_value()) {
		const char id = static_cast<char>(action);

		ActionParameter parameter = getFollowAim(toFollow, append);
		for (auto unit : formation->getUnits()) {
			if (unit != opt.value()) {
				StateManager::changeState(unit, UnitState::STOP);
			}
		}
		opt.value()->action(id, parameter);
	}
}
