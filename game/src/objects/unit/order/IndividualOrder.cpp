#include "IndividualOrder.h"
#include "Game.h"
#include "database/db_strcut.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/UnitConst.h"
#include "env/Environment.h"
#include "objects/unit/ActionParameter.h"

IndividualOrder::IndividualOrder(Unit* unit, UnitAction action, Urho3D::Vector2& vector, bool append):
	UnitOrder(static_cast<short>(action), append, vector), unit(unit) {
}

IndividualOrder::IndividualOrder(Unit* unit, UnitAction action, Physical* toUse, bool append):
	UnitOrder(static_cast<short>(action), append, toUse), unit(unit) {
	assert(toUse->isAlive());
	uidtest = toUse->uId.v;
}

bool IndividualOrder::expired() {
	return toUse != nullptr && !toUse->isAlive();
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
	unit->action(static_cast<UnitAction>(id), getTargetAim(unit->getMainGridIndex(), *vector));
	//TODO execute i akajca
	unit->resetFormation();
}

void IndividualOrder::addFollowAim() {
	const auto indexes = toUse->getIndexesForUse();
	if (indexes.empty()) {
		unit->action(static_cast<UnitAction>(id), getFollowAim(unit->getMainGridIndex(), indexes));
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
	//TODO to trzeba dobrze przemuœleæ
	std::vector<int> indexes = getIndexesToAct();

	if (!indexes.empty()) {
		const auto find = std::ranges::find(indexes, unit->getMainGridIndex());

		if (find == indexes.end()) {
			const auto param = getFollowAim(unit->getMainGridIndex(), indexes);
			if (param.aim != nullptr) {
				unit->action(UnitAction::FOLLOW, param);
				unit->addOrder(new IndividualOrder(unit, UnitAction(id), toUse, true));
			}
		} else {
			unit->action(static_cast<UnitAction>(id), ActionParameter(toUse));
		}
	}
}

std::vector<int> IndividualOrder::getIndexesToAct() const {
	//TODO to coœ przemyœleæ
	std::vector<int> indexes;
	if (static_cast<UnitAction>(id) == UnitAction::ATTACK) {
		if (unit->getDb()->typeRange) {
			if (toUse->belowRangeLimit()) {
				indexes = toUse->getIndexesForRangeUse(unit);
			}
		}
	}
	if (indexes.empty()) {
		//close and collect
		indexes = toUse->getIndexesForUse();
	}
	return indexes;
}
