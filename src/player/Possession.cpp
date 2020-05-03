#include "Possession.h"
#include <numeric>
#include "Game.h"
#include "Resources.h"
#include "database/DatabaseCache.h"
#include "math/VectorUtils.h"
#include "objects/ValueType.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/enums/UnitAction.h"
#include "objects/unit/state/StateManager.h"

Possession::Possession(char nation) {
	for (auto building : Game::getDatabase()->getNation(nation)->buildings) {
		auto id = building->id;
		if (buildingsPerId.size() <= id) {
			buildingsPerId.resize(id + 1, nullptr);
		}
		buildingsPerId[id] = new std::vector<Building*>();
	}
}

Possession::~Possession() {
	clear_vector(buildingsPerId);
}


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

std::vector<Building*>* Possession::getBuildings(short id) {
	return buildingsPerId[id];
}

void Possession::add(Building* building) {
	buildings.push_back(building);
	buildingsPerId[building->getId()]->push_back(building);
}

void Possession::add(Unit* unit) {
	units.push_back(unit);
	StateManager::checkChangeState(unit, UnitState::COLLECT);
	auto orders = Game::getDatabase()->getUnit(unit->getId())->orders;
	bool result = false;
	for (auto order : orders) {
		if (order->id == static_cast<char>(UnitAction::COLLECT)) {
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

	for (auto perId : buildingsPerId) {
		//TODO performance iterowac tylko jezeli ktos umarl - tylko jeœli to wyzej coœwyczyœci³o
		if (perId) {
			cleanDead(perId);
		}
	}

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
