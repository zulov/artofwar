#include "IndividualOrder.h"
#include "Game.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/Unit.h"
#include "simulation/env/Environment.h"


IndividualOrder::IndividualOrder(Unit* unit, const Urho3D::Vector2& vector,
                                 const Physical* toUse, UnitOrder action):
	FutureOrder(action, vector, toUse), unit(unit) {
}


IndividualOrder::~IndividualOrder() = default;

bool IndividualOrder::add(bool append) {
	unit->addAim(this, append);
	return false;
}

void IndividualOrder::addTargetAim() {
	unit->action(static_cast<char>(action), getTargetAim(unit->getMainCell(), vector)); //TODO execute i akajca
	static_cast<Unit*>(unit)->resetFormation();

	Game::getEnvironment()->invalidateCache();
}

void IndividualOrder::addFollowAim() {
}

void IndividualOrder::addChargeAim() {
}

void IndividualOrder::addAttackAim() {
}

void IndividualOrder::addDefendAim() {
	simpleAction();
}

void IndividualOrder::addDeadAim() {
	simpleAction();
}

void IndividualOrder::simpleAction() {
	unit->action(static_cast<char>(FutureOrder::action), ActionParameter());
}

// void IndividualAction::addChargeAim() {
// 	entity->action(static_cast<char>(action), getChargeAim(charge));
// }
//
// void IndividualAction::addFollowAim() {
// 	auto opt = toFollow->getPosToUseBy(static_cast<Unit*>(entity));
// 	if (opt.has_value()) {
// 		entity->action(static_cast<char>(action),
// 		               getFollowAim(entity->getMainCell(),
// 		                            opt.value(), toFollow));
// 	}
// }
//
// void IndividualAction::addAttackAim() {
// 	int a = 5;
// }
