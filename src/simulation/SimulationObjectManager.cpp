#include "SimulationObjectManager.h"
#include "Game.h"
#include "SimulationInfo.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include <algorithm>
#include <simulation/env/Enviroment.h>


SimulationObjectManager::SimulationObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	resources = new std::vector<ResourceEntity*>();

	buildingsToAdd = new std::vector<Building*>();
	resourcesToAdd = new std::vector<ResourceEntity*>();

	units->reserve(10000);
	buildings->reserve(100);
	resources->reserve(1000);

	toDisposePhysical.reserve(100);

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

void SimulationObjectManager::addAll(std::vector<Unit*>* _units) const {
	units->insert(units->end(), _units->begin(), _units->end());
}

void SimulationObjectManager::addAll(std::vector<Building*>* _buildings) const {
	buildings->insert(buildings->end(), _buildings->begin(), _buildings->end());
	buildingsToAdd->insert(buildingsToAdd->end(), _buildings->begin(), _buildings->end());
}

void SimulationObjectManager::addAll(std::vector<ResourceEntity*>* _resources) const {
	resources->insert(resources->end(), _resources->begin(), _resources->end());
	resourcesToAdd->insert(resourcesToAdd->end(), _resources->begin(), _resources->end());
}

void SimulationObjectManager::addUnits(unsigned int number, int id, Urho3D::Vector2& center,
                                       int player, int level) {
	updateUnits(unitFactory.create(number, id, center, player, level));
}

void SimulationObjectManager::addBuilding(int id, Urho3D::Vector2& center, int player,
                                          const Urho3D::IntVector2& _bucketCords, int level) {
	updateBuilding(buildingFactory.create(id, center, player, _bucketCords, level));
}


void SimulationObjectManager::addResource(int id, Urho3D::Vector2& center, const Urho3D::IntVector2& _bucketCords,
                                          int level) {
	updateResource(resourceFactory.create(id, center, _bucketCords, level));
}

template <class T>
void SimulationObjectManager::prepareToDispose(std::vector<T*>* objects) const {
	objects->erase( //TODO performance iterowac tylko jezeli ktos umarl - przemyslec to
	               std::remove_if(
	                              objects->begin(), objects->end(),
	                              physicalShouldDelete
	                             ),
	               objects->end());
}

void SimulationObjectManager::prepareToDispose() const {
	prepareToDispose(units);
	prepareToDispose(buildings);
	prepareToDispose(resources);
}

void SimulationObjectManager::load(dbload_unit* unit) {
	updateUnits(unitFactory.load(unit));
}

void SimulationObjectManager::load(dbload_building* building) {
	updateBuilding(buildingFactory.load(building));
}

void SimulationObjectManager::load(dbload_resource_entities* resource) {
	updateResource(resourceFactory.load(resource));
}

void SimulationObjectManager::updateUnits(std::vector<Unit*>* temp) {
	addAll(temp);
	if (!temp->empty()) {
		temp->clear();
		simulationInfo->setAmountUnitChanged();
	}
}

void SimulationObjectManager::updateBuilding(std::vector<Building*>* temp) {
	addAll(temp);
	if (!temp->empty()) {
		Game::getEnviroment()->update(buildingsToAdd);
		buildingsToAdd->clear();
		simulationInfo->setAmountBuildingChanged();
	}
}

void SimulationObjectManager::updateResource(std::vector<ResourceEntity*>* temp) {
	addAll(temp);
	if (!temp->empty()) {
		Game::getEnviroment()->update(resourcesToAdd);
		resourcesToAdd->clear();
		simulationInfo->setAmountResourceChanged();
	}
}

bool SimulationObjectManager::shouldDelete(Physical* physical) {
	if (physical->isToDispose()) {
		toDisposePhysical.push_back(physical);
		simulationInfo->setSthDied(physical->getType());
		return true;
	}
	physical->clean();
	return false;
}

void SimulationObjectManager::updateInfo(SimulationInfo* simulationInfo) const {
	simulationInfo->set(this->simulationInfo);
	simulationInfo->setUnitsNumber(units->size());
	this->simulationInfo->reset();
}

void SimulationObjectManager::dispose() {
	if (!toDisposePhysical.empty()) {
		clear_vector(toDisposePhysical);
	}
	simulationInfo->reset();
}
