#include "IndividualOrder.h"
#include "Game.h"
#include "consts.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/Unit.h"
#include "simulation/env/Environment.h"


IndividualOrder::IndividualOrder(Unit* unit, UnitAction action, const Urho3D::Vector2& vector,
                                 Physical* toUse, bool append):
	UnitOrder(action, append, vector, toUse), unit(unit) {
}

IndividualOrder::~IndividualOrder() = default;

bool IndividualOrder::expired()  {
	return toUse != nullptr && !toUse->isAlive();
}


bool IndividualOrder::add() {
	physical->addOrder(this);
	return false;
}

void IndividualOrder::addCollectAim() {
	followAndAct(9);
}

void IndividualOrder::addTargetAim() {
	physical->action(static_cast<char>(action), getTargetAim(physical->getMainCell(), vector)); //TODO execute i akajca
	static_cast<Unit*>(physical)->resetFormation();

	Game::getEnvironment()->invalidateCache();
}

void IndividualOrder::addFollowAim() {
	auto opt = toUse->getPosToUseBy(static_cast<Unit*>(physical));
	if (opt.has_value()) {
		physical->action(static_cast<char>(action),
		             getFollowAim(physical->getMainCell(),
		                          opt.value(), toUse));
	}
}

void IndividualOrder::addChargeAim() {
	physical->action(static_cast<char>(action), getChargeAim(vector));
}

void IndividualOrder::addAttackAim() {
	followAndAct(physical->getAttackRange());
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
	physical->action(static_cast<char>(action), Consts::EMPTY_ACTION_PARAMETER);
}

void IndividualOrder::followAndAct(float distThreshold) {
	auto posOpt = toUse->getPosToUseWithIndex(static_cast<Unit*>(physical));
	if (posOpt.has_value()) {
		auto postToUse = posOpt.value();
		if (std::get<2>(postToUse) != physical->getMainBucketIndex()) {
			auto pos = std::get<0>(postToUse);
			physical->action(static_cast<char>(UnitAction::FOLLOW),
			             getFollowAim(physical->getMainCell(),
			                          pos, toUse));
			physical->addOrder(new IndividualOrder(physical, action, {}, toUse, true)); //Dodanie celu po dojsciu
		} else {
			physical->action(static_cast<char>(action),
			             ActionParameter::Builder()
			             .setIndex(std::get<2>(postToUse))
			             .setThingsToInteract(toUse)
			             .build());
		}
	}
}
