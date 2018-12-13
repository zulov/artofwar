#include "IndividualAction.h"
#include "Game.h"
#include "objects/unit/Unit.h"
#include "simulation/env/Environment.h"

//
// IndividualAction::IndividualAction(Physical* entity, UnitOrder action, const Physical* paremeter, bool append)
// 	: ActionCommand(action, paremeter, nullptr, append), entity(entity) {
// }
//
// IndividualAction::IndividualAction(Physical* entity, UnitOrder action, const Urho3D::Vector2& vector, bool append) :
// 	ActionCommand(action, nullptr, new Urho3D::Vector2(vector), append), entity(entity),futureAim() {
// }

IndividualAction::~IndividualAction() = default;

IndividualAction::IndividualAction(Physical* entity, FutureAim& futureAim, bool append):
	ActionCommand(futureAim.action, nullptr, nullptr, append), entity(entity), futureAim(futureAim) {

}

Aim* IndividualAction::createAim(Unit* unit, FutureAim& nextAim) {
	ActionParameter ap = getTargetAim(unit->getMainCell(), nextAim.vector);
	//unit->action(static_cast<char>(nextAim.action), ap);
	unit->action(static_cast<char>(nextAim.action), ActionParameter());//TODO execute i akajca
	static_cast<Unit*>(unit)->resetFormation();

	Game::getEnvironment()->invalidateCache();
	return ap.aim;
}

void IndividualAction::addTargetAim(Urho3D::Vector2* to, bool append) {
	static_cast<Unit*>(entity)->addAim(futureAim, append);
	// entity->action(static_cast<char>(action), getTargetAim(entity->getMainCell(), *to));
	// static_cast<Unit*>(entity)->resetFormation();
	//
	// Game::getEnvironment()->invalidateCache();
}

void IndividualAction::addChargeAim(Urho3D::Vector2* charge, bool append) {
	entity->action(static_cast<char>(action), getChargeAim(charge));
}

void IndividualAction::addFollowAim(const Physical* toFollow, bool append) {
	auto opt = toFollow->getPosToUseBy(static_cast<Unit*>(entity));
	if (opt.has_value()) {
		entity->action(static_cast<char>(action),
		               getFollowAim(entity->getMainCell(),
		                            opt.value(), toFollow));
	}
}

void IndividualAction::addAttackAim(const Physical* physical, bool append) {
	int a = 5;
}

void IndividualAction::addDeadAim() {
	entity->action(static_cast<char>(action), ActionParameter());
}

void IndividualAction::addDefendAim() {
	entity->action(static_cast<char>(action), ActionParameter());
}
