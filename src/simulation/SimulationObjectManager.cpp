#include "SimulationObjectManager.h"
#include <algorithm>
#include <simulation/env/Enviroment.h>
#include "Game.h"


SimulationObjectManager::SimulationObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	resources = new std::vector<ResourceEntity*>();

	buildingsToAdd = new std::vector<Building*>();
	resourcesToAdd = new std::vector<ResourceEntity*>();

	units->reserve(10000);
	buildings->reserve(1000);
	resources->reserve(1000);
	toDispose.reserve(200);

	simulationInfo = new SimulationInfo();
}


SimulationObjectManager::~SimulationObjectManager() {
	clear_and_delete_vector(units);
	clear_and_delete_vector(buildings);
	clear_and_delete_vector(resources);

	delete buildingsToAdd;
	delete resourcesToAdd;

	delete simulationInfo;
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
	units->insert(units->end(), _units->begin(), _units->end());
}

void SimulationObjectManager::addAll(std::vector<Building*>* _buildings) {
	buildings->insert(buildings->end(), _buildings->begin(), _buildings->end());
	buildingsToAdd->insert(buildingsToAdd->end(), _buildings->begin(), _buildings->end());
}

void SimulationObjectManager::addAll(std::vector<ResourceEntity*>* _resources) {
	resources->insert(resources->end(), _resources->begin(), _resources->end());
	resourcesToAdd->insert(resourcesToAdd->end(), _resources->begin(), _resources->end());
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

void SimulationObjectManager::addUnits(unsigned int number, int id, Vector3* center,
                                       int player) {
	unitsTemp = unitFactory.create(number, id, center, player);
	updateUnits();
}

void SimulationObjectManager::addBuildings(int id, Vector3* center,
                                           int player, IntVector2 _bucketCords) {
	buildingsTemp = buildingFactory.create(id, center, player, _bucketCords);
	updateBuilding();
}


void SimulationObjectManager::addResources(int id, Vector3* center, IntVector2 _bucketCords) {
	resourcesTemp = resourceFactory.create(id, center, _bucketCords);
	updateResource();
}

void SimulationObjectManager::clean() {
	//if (simulationInfo->ifUnitDied()) {//TODO przemyslec to
	int prevSize = units->size();

	units->erase(
	             std::remove_if(
	                            units->begin(), units->end(),
	                            functionShouldDelete
	                           ),
	             units->end());
	if (units->size() != prevSize) {
		simulationInfo->setUnitDied();
	}
}

void SimulationObjectManager::load(dbload_unit* unit) {
	unitsTemp = unitFactory.load(unit);
	updateUnits();
}

void SimulationObjectManager::load(dbload_building* building) {
	buildingsTemp = buildingFactory.load(building);
	updateBuilding();
}

void SimulationObjectManager::load(dbload_resource_entities* resource) {
	resourcesTemp = resourceFactory.load(resource);
	updateResource();
}

void SimulationObjectManager::updateUnits() {
	addAll(unitsTemp);
	if (!unitsTemp->empty()) {
		simulationInfo->setAmountUnitChanged();
	}
}

void SimulationObjectManager::updateBuilding() {
	addAll(buildingsTemp);
	if (!buildingsTemp->empty()) {
		
		Game::get()->getEnviroment()->update(buildingsToAdd);
		buildingsToAdd->clear();
		simulationInfo->setAmountBuildingChanged();
	}
}

void SimulationObjectManager::updateResource() {
	addAll(resourcesTemp);
	if (!resourcesTemp->empty()) {
		Game::get()->getEnviroment()->update(resourcesToAdd);
		resourcesToAdd->clear();
		simulationInfo->setAmountResourceChanged();
	}
}

bool SimulationObjectManager::shouldDelete(Physical* physical) {
	if (!physical->isAlive()) {
		toDispose.push_back(physical);
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
	if (!toDispose.empty()) {
		clear_vector(toDispose);
	}
	simulationInfo->reset();
}
