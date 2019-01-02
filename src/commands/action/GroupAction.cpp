#include "GroupAction.h"
#include "Game.h"
#include "simulation/env/Environment.h"
#include "simulation/formation/FormationManager.h"

GroupAction::GroupAction(std::vector<Physical*>* entities, FutureOrder &futureAim, bool append)
	: ActionCommand(futureAim, append), entities(entities) {
}

void GroupAction::addAim() {
	auto opt = Game::getFormationManager()->createFormation(entities);
	if (opt.has_value()) {
		// if (!append) {
		// 	opt.value()->semiReset();
		// }
		opt.value()->addAim(futureAim, append);
	}
}

GroupAction::~GroupAction() = default;

// void GroupAction::addTargetAim(Urho3D::Vector2* to, bool append) {
// 	auto opt = Game::getFormationManager()->createFormation(entities);
// 	if (opt.has_value()) {
// 		if (!append) {
// 			opt.value()->semiReset();
// 		}
// 		opt.value()->addAim(*to, nullptr, action, append);
// 	}
// }
//
// void GroupAction::addChargeAim(Urho3D::Vector2* charge, bool append) {
// 	auto opt = Game::getFormationManager()->createFormation(entities);
// 	if (opt.has_value()) {
// 		if (!append) {
// 			opt.value()->semiReset();
// 		}
// 		opt.value()->addAim(*charge, nullptr, action, append);
// 	}
// }
//
// void GroupAction::addFollowAim(const Physical* toFollow, bool append) {
// 	auto opt = Game::getFormationManager()->createFormation(entities);
// 	if (opt.has_value()) {
// 		if (!append) {
// 			opt.value()->semiReset();
// 		}
// 		opt.value()->addAim({}, toFollow, action, append);
// 	}
// }
//
// void GroupAction::addAttackAim(const Physical* toAttack, bool append) {
// 	auto opt = Game::getFormationManager()->createFormation(entities);
// 	if (opt.has_value()) {
// 		if (!append) {
// 			opt.value()->semiReset();
// 		}
// 		opt.value()->addAim({}, toAttack, action, append);
// 	}
// }
//
// void GroupAction::addDeadAim() {
// 	for (auto entity : *entities) {
// 		entity->action(static_cast<char>(action), ActionParameter());
// 	}
// }
//
// void GroupAction::addDefendAim() {
// 	for (auto entity : *entities) {
// 		entity->action(static_cast<char>(action), ActionParameter());
// 	}
// }
