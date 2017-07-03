#include "SimulationObjectManager.h"


SimulationObjectManager::SimulationObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	resources = new std::vector<ResourceEntity*>();
	entities = new std::vector<Entity*>();
	
	units->reserve(10000);
	buildings->reserve(1000);
	resources->reserve(1000);
	entities->reserve(20000);

	unitFactory = new UnitFactory();
	buildingFactory = new BuildingFactory();
	resourceFactory = new ResourceFactory();
}


SimulationObjectManager::~SimulationObjectManager() {
	delete units;
	delete buildings;
	delete entities;
	delete resources;

	delete unitFactory;
	delete buildingFactory;
	delete resourceFactory;
}

void SimulationObjectManager::add(Unit* unit) {
	units->push_back(unit);
	add(static_cast<Entity*>(unit));
}

void SimulationObjectManager::add(Entity* entity) {
	AbstractObjectManager::add(entity);
}

void SimulationObjectManager::add(Building* building) {
	buildings->push_back(building);
	add(static_cast<Entity*>(building));
}

void SimulationObjectManager::add(ResourceEntity* resourceEntity) {
	resources->push_back(resourceEntity);
	add(static_cast<Entity*>(resourceEntity));
}

void SimulationObjectManager::addAll(std::vector<Unit*>* _units) {
	for (auto entity : (*_units)) {
		add(entity);
	}
}

void SimulationObjectManager::addAll(std::vector<Building*>* _buildings) {
	for (auto entity : (*_buildings)) {
		add(entity);
	}
}

void SimulationObjectManager::addAll(std::vector<Entity*>* _entities) {
	for (auto entity : (*_entities)) {
		add(entity);
	}
}

void SimulationObjectManager::addAll(std::vector<ResourceEntity*>* _resources) {
	for (auto entity : (*_resources)) {
		add(entity);
	}
}

std::vector<Unit*>* SimulationObjectManager::getUnits() {
	return units;
}

std::vector<Building*>* SimulationObjectManager::getBuildings() {
	return buildings;
}

std::vector<Entity*>* SimulationObjectManager::getEntities() {
	return entities;
}

std::vector<ResourceEntity*>* SimulationObjectManager::getResources() {
	return resources;
}

void SimulationObjectManager::addUnits(unsigned int number, UnitType unitType, Vector3* center, SpacingType spacingType, int player) {
	std::vector<Unit*>* newUnits = unitFactory->create(number, unitType, center, spacingType, player);
	addAll(newUnits);
	delete newUnits;
}

void SimulationObjectManager::addBuildings(unsigned int number, BuildingType buildingType, Vector3* center, SpacingType spacingType, int player) {
	std::vector<Building*>* newBuildings = buildingFactory->create(number, buildingType, center, spacingType);
	addAll(newBuildings);
	delete newBuildings;
}

void SimulationObjectManager::addResources(unsigned number, ResourceType resourceType, Vector3* center, SpacingType spacingType) {
	std::vector<ResourceEntity*>* newResources = resourceFactory->create(number, resourceType, center, spacingType);
	addAll(newResources);
	delete newResources;
}
