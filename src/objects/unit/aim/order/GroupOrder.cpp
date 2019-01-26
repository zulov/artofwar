#include "GroupOrder.h"


GroupOrder::GroupOrder(std::vector<Physical*>* entities, const Urho3D::Vector2& vector, const Physical* physical,
                       UnitOrder action): FutureOrder(action, vector, physical), entities(entities) {
}


GroupOrder::~GroupOrder() = default;

void GroupOrder::add(bool append) {
	//TODO to implement
}

void GroupOrder::addTargetAim() {
	// unit->action(static_cast<char>(action), getTargetAim(unit->getMainCell(), vector));//TODO execute i akajca
	// static_cast<Unit*>(unit)->resetFormation();
	//
	// Game::getEnvironment()->invalidateCache();
}

void GroupOrder::addFollowAim() {
}

void GroupOrder::addChargeAim() {
}

void GroupOrder::addAttackAim() {
}

void GroupOrder::addDefendAim() {
}

void GroupOrder::addDeadAim() {
}

//
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
// GroupAction::~GroupAction() = default;

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
