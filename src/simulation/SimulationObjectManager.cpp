#include "SimulationObjectManager.h"
#include "Game.h"
#include "SimulationInfo.h"
#include "objects/unit/Unit.h"
#include "objects/building/Building.h"
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

void SimulationObjectManager::addUnits(unsigned int number, int id, Urho3D::Vector2& center,
                                       int player, int level) {
	unitsTemp = unitFactory.create(number, id, center, player, level);
	updateUnits();
}

void SimulationObjectManager::addBuilding(int id, Urho3D::Vector2& center, int player,
                                          const Urho3D::IntVector2& _bucketCords, int level) {
	buildingsTemp = buildingFactory.create(id, center, player, _bucketCords, level);
	updateBuilding();
}


void SimulationObjectManager::addResource(int id, Urho3D::Vector2& center, const Urho3D::IntVector2& _bucketCords,
                                          int level) {
	resourcesTemp = resourceFactory.create(id, center, _bucketCords, level);
	updateResource();
}

void SimulationObjectManager::prepareUnitToDispose() const {
	units->erase( //TODO performance iterowac tylko jezeli ktos umarl - przemyslec to
	             std::remove_if(
	                            units->begin(), units->end(),
	                            physicalShouldDelete
	                           ),
	             units->end());
}

void SimulationObjectManager::prepareBuildingToDispose() const {
	buildings->erase(
	                 std::remove_if(
	                                buildings->begin(), buildings->end(),
	                                physicalShouldDelete
	                               ),
	                 buildings->end());
}

void SimulationObjectManager::prepareResourceToDispose() const {
	resources->erase(
	                 std::remove_if(
	                                resources->begin(), resources->end(),
	                                physicalShouldDelete
	                               ),
	                 resources->end());
}

void SimulationObjectManager::prepareToDispose() const {
	prepareUnitToDispose();
	prepareBuildingToDispose();
	prepareResourceToDispose();
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
		unitsTemp->clear();
		simulationInfo->setAmountUnitChanged();
	}
}

void SimulationObjectManager::updateBuilding() {
	addAll(buildingsTemp);
	if (!buildingsTemp->empty()) {

		Game::getEnviroment()->update(buildingsToAdd);
		buildingsToAdd->clear();
		simulationInfo->setAmountBuildingChanged();
	}
}

void SimulationObjectManager::updateResource() {
	addAll(resourcesTemp);
	if (!resourcesTemp->empty()) {
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
