#include "Possession.h"
#include "Game.h"
#include "Resources.h"
#include "database/DatabaseCache.h"
#include "math/SpanUtils.h"
#include "math/VectorUtils.h"
#include "objects/PhysicalUtils.h"
#include "objects/building/Building.h"
#include "simulation/ObjectsInfo.h"

Possession::Possession(char nation) {
	for (auto building : Game::getDatabase()->getNation(nation)->buildings) {
		auto id = building->id;
		if (buildingsPerId.size() <= id) {
			buildingsPerId.resize(id + 1, nullptr);
		}
		buildingsPerId[id] = new std::vector<Building*>();
	}
	resetSpan(unitsValuesAsSpan);
	resetSpan(freeArmyMetricsAsSpan);
	resetSpan(buildingsValuesAsSpan);
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
	return unitsValuesAsSpan[cast(value)];
}

float Possession::getBuildingsVal(BuildingMetric value) const {
	return buildingsValuesAsSpan[cast(value)];
}

std::vector<Unit*> Possession::getFreeArmy() {
	std::vector<Unit*> army(units.size());

	auto it = std::copy_if(units.begin(), units.end(), army.begin(), [](Unit* unit) {
		return isFreeSolider(unit);
	});
	army.resize(std::distance(army.begin(), it));
	return army;
}

void Possession::add(Building* building) {
	buildings.push_back(building);
	buildingsPerId[building->getId()]->push_back(building);
}

void Possession::add(Unit* unit) {
	units.push_back(unit);

	if (unit->getLevel()->canCollect) {
		workers.push_back(unit);
	}
}

void Possession::updateAndClean(Resources& resources, const ObjectsInfo* simInfo) {
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
	std::fill_n(freeArmyMetricsAsSpan.begin(), freeArmyMetricsAsSpan.size(), 0.f);
	for (auto unit : units) {
		unit->addValues(unitsValuesAsSpan);

		if (isFreeSolider(unit)) {
			unit->addValues(freeArmyMetricsAsSpan);
		}
	}

	std::fill_n(buildingsValuesAsSpan.begin(), buildingsValuesAsSpan.size(), 0.f);
	for (auto building : buildings) {
		building->addValues(buildingsValuesAsSpan);
	}

	resourcesSum = sumSpan(resources.getValues());

	freeWorkersNumber = std::count_if(workers.begin(), workers.end(), [](Unit* worker) {
		return isInFreeState(worker->getState());
	});
}
