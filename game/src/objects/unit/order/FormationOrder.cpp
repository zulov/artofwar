#include "FormationOrder.h"
#include "Game.h"
#include "IndividualOrder.h"
#include "objects/unit/Unit.h"
#include "enums/UnitAction.h"
#include "simulation/formation/Formation.h"
#include "env/Environment.h"

FormationOrder::FormationOrder(Formation* formation, short action,
                               Urho3D::Vector2& vector, bool append):
	UnitOrder(action, append, vector), formation(formation) {
}

FormationOrder::FormationOrder(Formation* formation, short action,
                               Physical* toUse, bool append):
	UnitOrder(action, append, toUse), formation(formation) {
}

bool FormationOrder::add() {
	//TODO to implement
	return false;
}

bool FormationOrder::expired() {
	return formation == nullptr
		|| formation->getSize() <= 0
		|| toUse != nullptr && !toUse->isAlive();
}

short FormationOrder::getSize() const {
	return formation->getSize();
}

void FormationOrder::addCollectAim() {
	followAndAct(9);
}

void FormationOrder::addTargetAim() {
	auto opt = formation->getLeader();
	if (opt.has_value() && vector) {
		Unit* leader = opt.value();
		leader->action(static_cast<UnitAction>(id), getTargetAim(leader->getMainGridIndex(), *vector));
		formation->stopAllBesideLeader();
		Game::getEnvironment()->invalidatePathCache();
	}
}

void FormationOrder::addFollowAim() {
	auto leaderOpt = formation->getLeader();
	if (leaderOpt.has_value()) {
		formation->stopAllBesideLeader();
		Unit* leader = leaderOpt.value();

		auto const indexes = toUse->getIndexesForUse(leader);
		if (!indexes.empty()) {
			leader->action(static_cast<UnitAction>(id), getFollowAim(leader->getMainGridIndex(), indexes));
		}
	}
}

void FormationOrder::addChargeAim() {
	auto action = static_cast<UnitAction>(id);
	for (auto unit : formation->getUnits()) {
		unit->action(action, getChargeAim(*vector));
	}
}

void FormationOrder::followAndAct(float distThreshold) {
	if (const auto optLeader = formation->getLeader(); optLeader.has_value()) {
		Unit* leader = optLeader.value();
		auto const indexes = toUse->getIndexesForUse(leader);
		if (!indexes.empty()) {
			if (Game::getEnvironment()->anyCloseEnough(indexes, leader->getMainGridIndex(), distThreshold)) {
				for (auto* unit : formation->getUnits()) {
					unit->resetFormation();
					unit->addOrder(new IndividualOrder(unit, UnitAction(id), toUse, false));
					//TODO to samo zrobic w innnych akcjach z atakiem
					//TOAttack jak nie ten to zaatakowac blizeszego
				}
				formation->remove();
			} else {
				//auto pos = std::get<0>(postToUse);
				leader->action(UnitAction::FOLLOW, getFollowAim(leader->getMainGridIndex(), indexes));
				formation->addOrder(new FormationOrder(formation, id, toUse, true));
				//Dodanie celu po dojsciu
			}
		}
	}
}

void FormationOrder::addAttackAim() {
	auto optLeader = formation->getLeader();
	if (optLeader.has_value()) {
		followAndAct(optLeader.value()->getAttackRange());
	}
}

void FormationOrder::addDefendAim() {
	simpleAction();
}

void FormationOrder::addDeadAim() {
	simpleAction();
}

void FormationOrder::addStopAim() {
	simpleAction();
}

void FormationOrder::simpleAction() const {
	const auto action = static_cast<UnitAction>(id);
	for (auto* unit : formation->getUnits()) {
		unit->resetFormation();
		unit->action(action);
	}
	formation->remove();
}
