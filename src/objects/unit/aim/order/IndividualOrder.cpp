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
	unit->addOrder(this);
	return false;
}

void IndividualOrder::addCollectAim() {
	followAndAct();
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
	followAndAct();
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

void IndividualOrder::followAndAct() {
	auto posOpt = toUse->getPosToUseWithIndex(static_cast<Unit*>(unit));
	if (posOpt.has_value()) {
		auto postToUse = posOpt.value();
		auto dist = std::get<1>(postToUse);
		if (dist > 25) {
			//TODO hardcode
			auto pos = std::get<0>(postToUse);
			unit->action(static_cast<char>(UnitOrder::FOLLOW),
			             getFollowAim(unit->getMainCell(),
			                          pos, toUse));
			unit->addOrder(new IndividualOrder(unit, action, {}, toUse, true)); //Dodanie celu po dojsciu
		} else {
			unit->addOrder(new IndividualOrder(unit, action, {}, toUse, false));
			//unit->interactWithOne(toUse, std::get<2>(postToUse), action);
		}

	}

}
