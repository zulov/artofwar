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
#include "simulation/SimulationInfo.h"

Possession::Possession(char nation) {
	for (auto building : Game::getDatabase()->getNation(nation)->buildings) {
		auto id = building->id;
		if (buildingsPerId.size() <= id) {
			buildingsPerId.resize(id + 1, nullptr);
		}
		buildingsPerId[id] = new std::vector<Building*>();
	}
	unitsValuesAsSpan = std::span{unitsValues};
	buildingsValuesAsSpan = std::span{buildingsValues};
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

float Possession::getUnitsVal(ValueType value) const {
	return unitsValues[static_cast<char>(value)];
}

float Possession::getBuildingsVal(ValueType value) const {
	return buildingsValues[static_cast<char>(value)];
}

std::vector<Unit*>& Possession::getWorkers() {
	return workers;
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

void Possession::updateAndClean(Resources& resources, SimulationInfo* simInfo) {
	cleanDead(buildings, simInfo->ifBuildingDied());
	cleanDead(units, simInfo->ifUnitDied());
	cleanDead(workers, simInfo->ifUnitDied());

	if (simInfo->ifBuildingDied()) {
		for (auto perId : buildingsPerId) {
			if (perId) {
				cleanDead(perId);
			}
		}
	}
	std::fill_n(unitsValues, magic_enum::enum_count<ValueType>(), 0.f);
	for (auto unit : units) {
		unit->addValues(unitsValuesAsSpan);
	}

	std::fill_n(buildingsValues, magic_enum::enum_count<ValueType>(), 0.f);
	for (auto building : buildings) {
		building->addValues(buildingsValuesAsSpan);
	}

	attackSum = buildingsValuesAsSpan[static_cast<char>(ValueType::ATTACK)]
		+ unitsValuesAsSpan[static_cast<char>(ValueType::ATTACK)];
	defenceSum = buildingsValuesAsSpan[static_cast<char>(ValueType::DEFENCE)]
		+ unitsValuesAsSpan[static_cast<char>(ValueType::DEFENCE)];

	auto values = resources.getValues();
	resourcesSum = std::accumulate(values.begin(), values.end(), 0.f);
}
