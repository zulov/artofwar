#include "Possession.h"
#include <numeric>
#include "Game.h"
#include "Resources.h"
#include "database/DatabaseCache.h"
#include "math/VectorUtils.h"
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

int Possession::getFreeWorkersNumber() const { return freeWorkersNumber; }

std::vector<Building*>* Possession::getBuildings(short id) {
	return buildingsPerId[id];
}

float Possession::getUnitsVal(UnitMetric value) const {
	return unitsMetrics[static_cast<char>(value)];
}

float Possession::getBuildingsVal(BuildingMetric value) const {
	return buildingsMetrics[static_cast<char>(value)];
}

std::span<float> Possession::getUnitsMetrics() const {
	return unitsValuesAsSpan;
}

std::span<float> Possession::getBuildingsMetrics() const {
	return buildingsValuesAsSpan;
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
	std::fill_n(unitsValuesAsSpan.begin(), unitsValuesAsSpan.size(), 0.f);
	for (auto unit : units) {
		unit->addValues(unitsValuesAsSpan);
	}

	std::fill_n(buildingsValuesAsSpan.begin(), buildingsValuesAsSpan.size(), 0.f);
	for (auto building : buildings) {
		building->addValues(buildingsValuesAsSpan);
	}

	auto values = resources.getValues();
	resourcesSum = std::accumulate(values.begin(), values.end(), 0.f);

	freeWorkersNumber = std::count_if(workers.begin(), workers.end(), [](Unit* worker) {
		return isNotInStates(worker->getState(), {UnitState::COLLECT, UnitState::GO_TO});
	});
}
