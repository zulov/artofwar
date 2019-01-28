#include "GroupOrder.h"
#include "FormationOrder.h"
#include "Game.h"
#include "objects/unit/ActionParameter.h"
#include "simulation/formation/FormationManager.h"


GroupOrder::GroupOrder(std::vector<Physical*>* entities, UnitOrder action, const Urho3D::Vector2& vector,
                       const Physical* physical, bool append):
	FutureOrder(action, append, vector, physical), entities(entities) {
}

GroupOrder::~GroupOrder() = default;

bool GroupOrder::add() {
	execute();
	return true;
}

void GroupOrder::addTargetAim() {
	auto opt = Game::getFormationManager()->createFormation(entities);
	if (opt.has_value()) {
		opt.value()->addAim(new FormationOrder(opt.value(), action, vector, nullptr, append));
	}
}

void GroupOrder::addFollowAim() {
}

void GroupOrder::addChargeAim() {
}

void GroupOrder::addAttackAim() {
}

void GroupOrder::addDefendAim() {
	simpleAction();
}

void GroupOrder::addDeadAim() {
	simpleAction();
}

void GroupOrder::simpleAction() {
	for (auto entity : *entities) {
		entity->action(static_cast<char>(FutureOrder::action), ActionParameter());
	}
}

// void GroupAction::addAim() {
// 	auto opt = Game::getFormationManager()->createFormation(entities);
// 	if (opt.has_value()) {
// 		// if (!append) {
// 		// 	opt.value()->semiReset();
// 		// }
// 		opt.value()->addAim(futureAim, append);
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
