#include "IndividualOrder.h"
#include "Game.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "simulation/env/Environment.h"
#include "objects/unit/ActionParameter.h"

IndividualOrder::IndividualOrder(Unit* unit, UnitAction action, Urho3D::Vector2& vector, bool append):
	UnitOrder(static_cast<short>(action), append, vector), unit(unit) {}

IndividualOrder::IndividualOrder(Unit* unit, UnitAction action, Physical* toUse, bool append):
	UnitOrder(static_cast<short>(action), append, toUse), unit(unit) {}

bool IndividualOrder::expired() {
	return unit == nullptr
		|| toUse != nullptr && !toUse->isAlive();
}

bool IndividualOrder::add() {
	unit->addOrder(this);
	return false;
}

short IndividualOrder::getSize() const {
	return 1;
}

void IndividualOrder::addCollectAim() {
	followAndAct();
}

void IndividualOrder::addTargetAim() {
	unit->action(static_cast<UnitAction>(id), getTargetAim(unit->getMainCell(), *vector)); //TODO execute i akajca
	unit->resetFormation();

	Game::getEnvironment()->invalidatePathCache();
}

void IndividualOrder::addFollowAim() {
	auto opt = toUse->getPosToUseWithIndex(unit);
	if (opt.has_value()) {
		unit->action(static_cast<UnitAction>(id), getFollowAim(unit->getMainCell(),std::get<2>(opt.value())));
	}
}

void IndividualOrder::addChargeAim() {
	unit->action(static_cast<UnitAction>(id), getChargeAim(*vector));
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

void IndividualOrder::addStopAim() {
	simpleAction();
}

void IndividualOrder::simpleAction() const {
	unit->action(static_cast<UnitAction>(id));
}

void IndividualOrder::followAndAct() {
	auto posOpt = toUse->getPosToUseWithIndex(unit);
	if (posOpt.has_value()) {
		auto postToUse = posOpt.value();
		if (std::get<2>(postToUse) != unit->getMainBucketIndex()) {
			//TODO moga byc tez inne pozycje nie tylko ta jedna 
			const auto param = getFollowAim(unit->getMainCell(), std::get<2>(postToUse));
			if(param.aim!=nullptr) {
				unit->action(UnitAction::FOLLOW, param);
				unit->addOrder(new IndividualOrder(unit, UnitAction(id), toUse, true));
			}
		} else {
			unit->action(static_cast<UnitAction>(id), ActionParameter(toUse, std::get<2>(postToUse)));
		}
	}
}
