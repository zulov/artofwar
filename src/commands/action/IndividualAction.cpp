#include "IndividualAction.h"
#include "Game.h"
#include "objects/unit/Unit.h"
#include "simulation/env/Environment.h"

IndividualAction::~IndividualAction() = default;

IndividualAction::IndividualAction(Physical* entity, FutureOrder& futureAim, bool append): entity(entity) {
}

Aim* IndividualAction::createAim(Unit* unit, FutureOrder& nextAim) {
	// switch (nextAim.action) { case UnitOrder::GO: break;
	// case UnitOrder::STOP: break;
	// case UnitOrder::CHARGE: break;
	// case UnitOrder::ATTACK: break;
	// case UnitOrder::DEAD: break;
	// case UnitOrder::DEFEND: break;
	// case UnitOrder::FOLLOW: break;
	// case UnitOrder::COLLECT: break;
	// default: ;
	// }


	// ActionParameter ap = getTargetAim(unit->getMainCell(), nextAim.vector);
	// //unit->action(static_cast<char>(nextAim.action), ap);
	// unit->action(static_cast<char>(nextAim.action), ActionParameter());//TODO execute i akajca
	// static_cast<Unit*>(unit)->resetFormation();
	//
	// Game::getEnvironment()->invalidateCache();
	// return ap.aim;
}

void IndividualAction::addAim() {
	//static_cast<Unit*>(entity)->addAim(futureAim, append);
	// entity->action(static_cast<char>(action), getTargetAim(entity->getMainCell(), *to));
	// static_cast<Unit*>(entity)->resetFormation();
	//
	// Game::getEnvironment()->invalidateCache();
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
//
// void IndividualAction::addDeadAim() {
// 	entity->action(static_cast<char>(action), ActionParameter());
// }
//
// void IndividualAction::addDefendAim() {
// 	entity->action(static_cast<char>(action), ActionParameter());
// }
