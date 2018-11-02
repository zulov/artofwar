#include "GroupAction.h"
#include "Game.h"
#include "simulation/env/Environment.h"
#include "simulation/formation/FormationManager.h"

GroupAction::GroupAction(std::vector<Physical*>* entities, UnitOrder action, Urho3D::Vector2* vector, bool append)
	: ActionCommand(action, nullptr, vector, append), entities(entities) {
}

GroupAction::GroupAction(std::vector<Physical*>* entities, UnitOrder action, Physical* thing, bool append)
	: ActionCommand(action, thing, nullptr, append), entities(entities) {
}

GroupAction::~GroupAction() = default;

void GroupAction::addTargetAim(Urho3D::Vector2* to, bool append) {
	auto opt = Game::getFormationManager()->createFormation(entities);
	if (opt.has_value()) {
		if (!append) {
			opt.value()->semiReset();
		}
		opt.value()->addAim(*to, nullptr, action, append);
	}
}

void GroupAction::addChargeAim(Urho3D::Vector2* charge, bool append) {
	auto opt = Game::getFormationManager()->createFormation(entities);
	if (opt.has_value()) {
		if (!append) {
			opt.value()->semiReset();
		}
		opt.value()->addAim(*charge, nullptr, action, append);
	}
}

void GroupAction::addFollowAim(const Physical* toFollow, bool append) {
	auto opt = Game::getFormationManager()->createFormation(entities);
	if (opt.has_value()) {
		if (!append) {
			opt.value()->semiReset();
		}
		opt.value()->addAim({}, toFollow, action, append);
	}
}

void GroupAction::addDeadAim() {
	int a = 5;
}
