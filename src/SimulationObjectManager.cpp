#include "SimulationObjectManager.h"


SimulationObjectManager::SimulationObjectManager() {
	units = new std::vector<Unit*>();
	buildings = new std::vector<Building*>();
	entities = new std::vector<Entity*>();
	unitFactory = new UnitFactory();
	buildingFactory = new BuildingFactory();
}


SimulationObjectManager::~SimulationObjectManager() {
	delete units;
	delete buildings;
	delete entities;

	delete unitFactory;
	delete buildingFactory;
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

void SimulationObjectManager::addAll(std::vector<Unit*>* _units) {
	for (int i = 0; i < _units->size(); ++i) {
		add(_units->at(i));
	}
}

void SimulationObjectManager::addAll(std::vector<Building*>* _buildings) {

}

void SimulationObjectManager::addAll(std::vector<Entity*>* _entities) {
	for (int i = 0; i < _entities->size(); ++i) {
		add(_entities->at(i));
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

std::vector<Unit*>* SimulationObjectManager::createUnits(unsigned int number, UnitType unitType, Vector3* center, SpacingType spacingType) {
	std::vector<Unit*>* newUnits = unitFactory->createUnits(number, unitType, center, spacingType);
	addAll(newUnits);
	delete newUnits;
	return units;
}
