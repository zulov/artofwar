#include "IndividualOrder.h"
#include "Game.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/Unit.h"
#include "objects/order/enums/UnitAction.h"
#include "objects/order/enums/UnitActionType.h"
#include "simulation/env/Environment.h"


IndividualOrder::IndividualOrder(Unit* unit, UnitAction action,
                                 Urho3D::Vector2& vector, bool append):
	UnitOrder(UnitActionType::ORDER, static_cast<short>(action), append, vector),
	unit(unit) {
}

IndividualOrder::IndividualOrder(Unit* unit, UnitAction action,
                                 Physical* toUse, bool append):
	UnitOrder(UnitActionType::ORDER, static_cast<short>(action), append, toUse), unit(unit) {
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
	unit->action(static_cast<UnitAction>(id), getTargetAim(unit->getMainCell(), *vector)); //TODO execute i akajca
	unit->resetFormation();

	Game::getEnvironment()->invalidateCache();
}

void IndividualOrder::addFollowAim() {
	auto opt = toUse->getPosToUseBy(unit);
	if (opt.has_value()) {
		unit->action(static_cast<UnitAction>(id), getFollowAim(unit->getMainCell(),
		                                                       opt.value(), toUse));
	}
}

void IndividualOrder::addChargeAim() {
	unit->action(static_cast<UnitAction>(id), getChargeAim(*vector));
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
	unit->action(static_cast<UnitAction>(id));
}

void IndividualOrder::followAndAct(float distThreshold) {
	auto posOpt = toUse->getPosToUseWithIndex(unit);
	if (posOpt.has_value()) {
		auto postToUse = posOpt.value();
		if (std::get<2>(postToUse) != unit->getMainBucketIndex()) {
			auto pos = std::get<0>(postToUse);
			unit->action(UnitAction::FOLLOW,
			             getFollowAim(unit->getMainCell(), pos, toUse));
			unit->addOrder(new IndividualOrder(unit, UnitAction(id), toUse, true));
			//Dodanie celu po dojsciu
		} else {
			unit->action(static_cast<UnitAction>(id),
			             ActionParameter::Builder()
			             .setIndex(std::get<2>(postToUse))
			             .setThingsToInteract(toUse)
			             .build());
		}
	}
}