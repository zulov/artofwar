#include "IndividualOrder.h"
#include "Game.h"
#include "consts.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/Unit.h"
#include "objects/order/enums/UnitAction.h"
#include "simulation/env/Environment.h"


IndividualOrder::IndividualOrder(Unit* unit, UnitActionType actionType, UnitAction action,
                                 const Urho3D::Vector2& vector, Physical* toUse, bool append):
	UnitOrder(actionType, action, append, toUse, vector), unit(unit) {
}

IndividualOrder::~IndividualOrder() = default;

bool IndividualOrder::expired() {
	return unit == nullptr || (toUse != nullptr && !toUse->isAlive());
}

bool IndividualOrder::add() {
	unit->addOrder(this);
	return false;
}

void IndividualOrder::clean() {
	if (!unit->isAlive()) {
		unit = nullptr;
	}
}

void IndividualOrder::addCollectAim() {
	followAndAct(9);
}

void IndividualOrder::addTargetAim() {
	unit->action(static_cast<char>(action), getTargetAim(unit->getMainCell(), vector)); //TODO execute i akajca
	unit->resetFormation();

	Game::getEnvironment()->invalidateCache();
}

void IndividualOrder::addFollowAim() {
	auto opt = toUse->getPosToUseBy(unit);
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
	followAndAct(unit->getAttackRange());
}

void IndividualOrder::addDefendAim() {
	simpleAction();
}

void IndividualOrder::addDeadAim() {
	simpleAction();
}

void IndividualOrder::addStopAim() {
	simpleAction();
}

void IndividualOrder::simpleAction() const {
	unit->action(static_cast<char>(action), Consts::EMPTY_ACTION_PARAMETER);
}

void IndividualOrder::followAndAct(float distThreshold) {
	auto posOpt = toUse->getPosToUseWithIndex(unit);
	if (posOpt.has_value()) {
		auto postToUse = posOpt.value();
		if (std::get<2>(postToUse) != unit->getMainBucketIndex()) {
			auto pos = std::get<0>(postToUse);
			unit->action(static_cast<char>(UnitAction::FOLLOW),
			             getFollowAim(unit->getMainCell(),
			                          pos, toUse));
			unit->addOrder(new IndividualOrder(unit, UnitActionType::ORDER, UnitAction(action), {}, toUse, true));
			//Dodanie celu po dojsciu
		} else {
			unit->action(static_cast<char>(action),
			             ActionParameter::Builder()
			             .setIndex(std::get<2>(postToUse))
			             .setThingsToInteract(toUse)
			             .build());
		}
	}
}
