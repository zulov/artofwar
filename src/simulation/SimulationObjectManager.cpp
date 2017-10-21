#include "SimulationObjectManager.h"
#include <algorithm>


SimulationObjectManager::SimulationObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	resources = new std::vector<ResourceEntity*>();
	toDispose = new std::vector<Physical*>();

	unitsToAdd = new std::vector<Unit*>();
	buildingsToAdd = new std::vector<Building*>();;
	resourcesToAdd = new std::vector<ResourceEntity*>();

	units->reserve(10000);
	buildings->reserve(1000);
	resources->reserve(1000);
	toDispose->reserve(1000);

	unitFactory = new UnitFactory();
	buildingFactory = new BuildingFactory();
	resourceFactory = new ResourceFactory();

	simulationInfo = new SimulationInfo();
}


SimulationObjectManager::~SimulationObjectManager() {
	clear_and_delete_vector(units);
	clear_and_delete_vector(buildings);
	clear_and_delete_vector(resources);
	clear_and_delete_vector(toDispose);

	delete unitFactory;
	delete buildingFactory;
	delete resourceFactory;
}

void SimulationObjectManager::add(Unit* unit) {
	units->push_back(unit);
}

void SimulationObjectManager::add(Building* building) {
	buildings->push_back(building);
}

void SimulationObjectManager::add(ResourceEntity* resourceEntity) {
	resources->push_back(resourceEntity);
}

void SimulationObjectManager::addAll(std::vector<Unit*>* _units) {
	units->insert(std::end(*units), std::begin(*_units), std::end(*_units));
	//unitsToAdd->insert(std::end(*unitsToAdd), std::begin(*_units), std::end(*_units));
}

void SimulationObjectManager::addAll(std::vector<Building*>* _buildings) {
	buildings->insert(std::end(*buildings), std::begin(*_buildings), std::end(*_buildings));
	buildingsToAdd->insert(std::end(*buildingsToAdd), std::begin(*_buildings), std::end(*_buildings));
}

void SimulationObjectManager::addAll(std::vector<ResourceEntity*>* _resources) {
	resources->insert(std::end(*resources), std::begin(*_resources), std::end(*_resources));
	resourcesToAdd->insert(std::end(*resourcesToAdd), std::begin(*_resources), std::end(*_resources));
}

std::vector<Unit*>* SimulationObjectManager::getUnits() {
	return units;
}

std::vector<Building*>* SimulationObjectManager::getBuildings() {
	return buildings;
}

std::vector<ResourceEntity*>* SimulationObjectManager::getResources() {
	return resources;
}

vector<Unit*>* SimulationObjectManager::getUnitsToAdd() {
	return unitsToAdd;
}

vector<Building*>* SimulationObjectManager::getBuildingsToAdd() {
	return buildingsToAdd;
}

vector<ResourceEntity*>* SimulationObjectManager::getResourcesToAdd() {
	return resourcesToAdd;
}

void SimulationObjectManager::addUnits(unsigned int number, int id, Vector3* center,
                                       int player) {
	unitsTemp = unitFactory->create(number, id, center, player);
	addAll(unitsTemp);
	if (!unitsTemp->empty()) {
		simulationInfo->setAmountUnitChanged();
	}
}

void SimulationObjectManager::addBuildings(int id, Vector3* center,
                                           int player) {
	buildingsTemp = buildingFactory->create(id, center, player);
	addAll(buildingsTemp);
	if (!buildingsTemp->empty()) {
		simulationInfo->setAmountBuildingChanged();
	}
}

void SimulationObjectManager::addResources(int id, Vector3* center) {
	resourcesTemp = resourceFactory->create(id, center);
	addAll(resourcesTemp);
	if (!resourcesTemp->empty()) {
		simulationInfo->setAmountResourceChanged();
	}
}

void SimulationObjectManager::clearUnitsToAdd() {
	unitsToAdd->clear();
}

void SimulationObjectManager::clearBuildingsToAdd() {
	buildingsToAdd->clear();
}

void SimulationObjectManager::clearResourcesToAdd() {
	resourcesToAdd->clear();
}

void SimulationObjectManager::clean() {
	//if (simulationInfo->ifUnitDied()) {//TODO przemyslec to
	int prevSize = units->size();
	std::function<bool(Physical*)> function = std::bind(&SimulationObjectManager::shouldDelete, this, placeholders::_1);

	units->erase(
	             std::remove_if(
	                            units->begin(), units->end(),
	                            function
	                           ),
	             units->end());
	if (units->size() != prevSize) {
		simulationInfo->setUnitDied();
	}
}

bool SimulationObjectManager::shouldDelete(Physical* physical) {
	if (!physical->isAlive()) {
		toDispose->push_back(physical);
		return true;
	}
	physical->clean();
	return false;
}

void SimulationObjectManager::updateInfo(SimulationInfo* simulationInfo) {
	simulationInfo->set(this->simulationInfo);
	this->simulationInfo->reset();
}

void SimulationObjectManager::dispose() {
	if (!toDispose->empty()) {
		clear_vector(toDispose);
	}
	simulationInfo->reset();
}
