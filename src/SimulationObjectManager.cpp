#include "SimulationObjectManager.h"
#include <algorithm>


SimulationObjectManager::SimulationObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	resources = new std::vector<ResourceEntity*>();
	toDispose = new std::vector<Physical*>();

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
}

void SimulationObjectManager::addAll(std::vector<Building*>* _buildings) {
	buildings->insert(std::end(*buildings), std::begin(*_buildings), std::end(*_buildings));
}

void SimulationObjectManager::addAll(std::vector<ResourceEntity*>* _resources) {
	resources->insert(std::end(*resources), std::begin(*_resources), std::end(*_resources));
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

void SimulationObjectManager::addUnits(unsigned int number, int id, Vector3* center, SpacingType spacingType, int player) {
	tempUnits = unitFactory->create(number, id, center, spacingType, player);
	addAll(tempUnits);
	simulationInfo->setAmountUnitChanged();
	tempUnits->clear();
}

void SimulationObjectManager::addBuildings(unsigned int number, int id, Vector3* center, SpacingType spacingType, int player) {
	tempBuildings = buildingFactory->create(number, id, center, spacingType);
	addAll(tempBuildings);
	simulationInfo->setAmountBuildingChanged();
	tempBuildings->clear();
}

void SimulationObjectManager::addResources(unsigned number, int id, Vector3* center, SpacingType spacingType) {
	tempResources = resourceFactory->create(number, id, center, spacingType);
	addAll(tempResources);
	simulationInfo->setAmountResourceChanged();
	tempResources->clear();
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
	clear_vector(toDispose);
	simulationInfo->reset();
}
