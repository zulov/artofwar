#include "Possession.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "Resources.h"
#include "objects/unit/state/StateManager.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "UnitOrder.h"
#include "VectorUtils.h"
#include "objects/ValueType.h"
#include <numeric>

int Possession::getScore() const {
	return buildings.size() * 10 + units.size() + resourcesSum / 100;
}

int Possession::getUnitsNumber() const {
	return units.size();
}

int Possession::getBuildingsNumber() const {
	return buildings.size();
}

int Possession::getWorkersNumber() const {
	return workers.size();
}

int Possession::getAttackScore() const {
	return attackSum;
}

int Possession::getDefenceScore() const {
	return defenceSum;
}

void Possession::add(Building* building) {
	buildings.push_back(building);
}

void Possession::add(Unit* unit) {
	units.push_back(unit);
	StateManager::checkChangeState(unit, UnitState::COLLECT);
	auto orders = Game::getDatabase()->getOrdersForUnit(unit->getDbID());
	bool result = false;
	for (auto order : *orders) {
		if (order->id == static_cast<char>(UnitOrder::COLLECT)) {
			result = true;
		}
	}
	if (result) {
		workers.push_back(unit);
	}
}

void Possession::updateAndClean(Resources& resources) {
	cleanDead(buildings); //TODO performance iterowac tylko jezeli ktos umarl - przemyslec to
	cleanDead(units); //TODO performance iterowac tylko jezeli ktos umarl - przemyslec to
	cleanDead(workers); //TODO performance iterowac tylko jezeli ktos umarl - przemyslec to

	attackSum = 0;
	defenceSum = 0;
	for (auto unit : units) {
		attackSum += unit->getValueOf(ValueType::ATTACK);
		defenceSum += unit->getValueOf(ValueType::DEFENCE);
	}
	for (auto building : buildings) {
		attackSum += building->getValueOf(ValueType::ATTACK);
		defenceSum += building->getValueOf(ValueType::DEFENCE);
	}

	short size = resources.getSize();
	float* values = resources.getValues();
	resourcesSum = std::accumulate(values, values + size, 0.f);
}
