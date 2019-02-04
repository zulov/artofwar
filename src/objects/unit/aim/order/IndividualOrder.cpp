#include "IndividualOrder.h"
#include "Game.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/Unit.h"
#include "simulation/env/Environment.h"


IndividualOrder::IndividualOrder(Unit* unit, UnitOrder action, const Urho3D::Vector2& vector,
                                 const Physical* toUse, bool append):
	FutureOrder(action, append, vector, toUse), unit(unit) {
}


IndividualOrder::~IndividualOrder() = default;

bool IndividualOrder::add() {
	unit->addOrder(this, append);
	return false;
}

void IndividualOrder::addCollectAim() {

}

void IndividualOrder::addTargetAim() {
	unit->action(static_cast<char>(action), getTargetAim(unit->getMainCell(), vector)); //TODO execute i akajca
	static_cast<Unit*>(unit)->resetFormation();

	Game::getEnvironment()->invalidateCache();
}

void IndividualOrder::addFollowAim() {
	auto opt = toUse->getPosToUseBy(static_cast<Unit*>(unit));
	if (opt.has_value()) {
		unit->action(static_cast<char>(action),
		             getFollowAim(unit->getMainCell(),
		                          opt.value(), toUse));
	}
}

void IndividualOrder::addChargeAim() {
	unit->action(static_cast<char>(action), getChargeAim(vector));
}

void IndividualOrder::addAttackAim() {
	int a = 5;
}

void IndividualOrder::addDefendAim() {
	simpleAction();
}

void IndividualOrder::addDeadAim() {
	simpleAction();
}

void IndividualOrder::simpleAction() const {
	unit->action(static_cast<char>(action), ActionParameter());
}
